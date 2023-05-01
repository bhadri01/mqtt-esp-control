from handGesture import hand
import cv2
import json
import math
import paho.mqtt.client as mqtt



def findPercents(inp, mi, ma, v):
    va = (inp - mi) * 100 / (ma - mi)
    if v == 100:
        va = v - va
    if va > 100:
        return 100
    elif va < 0:
        return 0
    else:
        return int(va)


def CAR(rightHand, img):
    lis = []
    if len(rightHand) > 0:
        x0, x1 = rightHand[0][0], rightHand[12][0]
        y0, y1 = rightHand[0][1], rightHand[12][1]
        x3, x4 = rightHand[3][0], rightHand[4][0]
        acc = findPercents(math.hypot(x0-x1, y0-y1), 50, 140, 0)
        if acc > 0:
            angle = abs(math.atan2(y1 - y0, x1 - x0) * 180 / math.pi)
            lis.append(acc)
            lis.append(angle)
            lis.append(-1)
            if angle < 60:
                cv2.putText(img, "right direction", (30, 40),
                            cv2.FONT_HERSHEY_PLAIN, 2, (50, 255, 0), 2)
            elif angle > 120:
                cv2.putText(img, "left direction", (30, 40),
                            cv2.FONT_HERSHEY_PLAIN, 2, (50, 255, 0), 2)
            else:
                if x3 > x4:
                    cv2.putText(img, "forward direction", (30, 40),
                                cv2.FONT_HERSHEY_PLAIN, 2, (50, 255, 0), 2)
                    lis[2] = 1
                else:
                    cv2.putText(img, "backward direction", (30, 40),
                                cv2.FONT_HERSHEY_PLAIN, 2, (50, 255, 0), 2)
                    lis[2] = 0
        else:
            return lis
        # circle shape x and y axis point
        cv2.circle(img, (x0, y0), 8, (0, 255, 0), cv2.FILLED)
        cv2.circle(img, (x1, y1), 8, (0, 255, 0), cv2.FILLED)
        # line between the two points
        cv2.line(img, (x0, y0), (x1, y1), (255, 255, 255), 2)

        return lis
    else:
        return lis

broker_address = "rabbitmq.youngstorage.in"  # Replace with your MQTT broker address
topic = "car_receive"

client = mqtt.Client()
client.connect(broker_address)



hands = hand.Hand(max_hands=1)
cap = cv2.VideoCapture(0)
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break
    res = hand.DetectHands(frame, hands)
    if not res['status']:
        break
    img = res["image"]
    data = res["data"]
    # print(data)
    if "right" in data and len(data["right"]):
        rgbdata = CAR(data["right"], img)
        # print(rgbdata)
        client.publish(topic, json.dumps(rgbdata))
        print(json.dumps(rgbdata))
    else:
        client.publish(topic, json.dumps([]))
    # Display the resulting image
    cv2.imshow('Hand Gestures', img)
    if cv2.waitKey(1) == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()
client.disconnect()