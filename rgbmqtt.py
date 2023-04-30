import asyncio
import websockets
from handGesture import hand
import cv2
import mediapipe as mp
import json
import math
import paho.mqtt.client as mqtt


def findPercents(inp, dist, v):
    max_dist = 180  # maximum distance in centimeters
    scale_range = 255
    scale_min = 0.5  # minimum scaling percentage
    scale_max = 0.9  # maximum scaling percentage

    scale_factor = scale_range / (max_dist * (scale_max - scale_min))
    scaled_min = scale_factor * (dist * scale_min)
    scaled_max = scale_factor * (dist * scale_max)

    va = (inp - scaled_min) * scale_range / (scaled_max - scaled_min)

    if v == 255:
        va = v - va
    if va > 255:
        return 255
    elif va < 0:
        return 0
    return int(va)


def RGB(right, img):
    print("RGB Effect like Doctor Strange")
    # rgb x and y axis point
    x0, y0 = right[0][0], right[0][1]
    rx, ry = right[4][0], right[4][1]
    gx, gy = right[8][0], right[8][1]
    bx, by = right[12][0], right[12][1]

    # circle shape x and y axis point
    cv2.circle(img, (rx, ry), 8, (0, 0, 255), cv2.FILLED)
    cv2.circle(img, (gx, gy), 8, (0, 255, 0), cv2.FILLED)
    cv2.circle(img, (bx, by), 8, (255, 0, 0), cv2.FILLED)

    # lines for the eache shape in rgb
    cv2.line(img, (x0, y0), (rx, ry), (0, 0, 255), 2)
    cv2.line(img, (x0, y0), (gx, gy), (0, 255, 0), 2)
    cv2.line(img, (x0, y0), (bx, by), (255, 0, 0), 2)
    # connect in bellow bottom point of index 0
    cv2.circle(img, (x0, y0), 8, (255, 255, 255), cv2.FILLED)
    Rlen = findPercents(math.hypot(rx - x0, ry - y0), 40, 0)
    Glen = findPercents(math.hypot(gx - x0, gy - y0), 50, 0)
    Blen = findPercents(math.hypot(bx - x0, by - y0), 50, 0)
    rgb = [Rlen, Glen, Blen]
    print(math.hypot(gx - x0, gy - y0))
    return rgb

broker_address = "localhost"  # Replace with your MQTT broker address
topic = "my_topic"

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
        rgbdata = RGB(data["right"], img)
        # print(rgbdata)
        client.publish(topic, json.dumps(rgbdata))
        print(json.dumps(rgbdata))
    # Display the resulting image
    cv2.imshow('Hand Gestures', img)
    if cv2.waitKey(1) == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()
client.disconnect()