import cv2
import mediapipe as mp
from google.protobuf.json_format import MessageToDict
import sys


def Hand(
        image_mode=False,
        max_hands=2,
        complexity=1,
        detection_confidence=0.5,
        tracking_confidence=0.5):
    # videocapture initialization
    mpHande = mp.solutions.hands
    hands = mpHande.Hands(
        static_image_mode=image_mode,
        max_num_hands=max_hands,
        model_complexity=complexity,
        min_detection_confidence=detection_confidence,
        min_tracking_confidence=tracking_confidence,
    )
    return hands


def DetectHands(
    img,  # cv image from the camera
    hands,
    handshow=True
):
    # first try to initialization the socket server
    try:
        # pTime = cTime = 0
        img = cv2.flip(img, 1)
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        result = hands.process(imgRGB)
        left,right = [],[]
        # find the hand and position with its 20 points
        if result.multi_hand_landmarks:
            # if both hand detected this code will happen
            if len(result.multi_handedness) == 2:
                for i in range(len(result.multi_handedness)):
                    label = MessageToDict(result.multi_handedness[i])[
                        'classification'][0]['label']
                    for id, lm in enumerate(result.multi_hand_landmarks[i].landmark):
                        h, w, c = img.shape
                        cx, cy = int(lm.x*w), int(lm.y*h)
                        # Id, x, y axis of the point
                        if label == "Left":
                            left.append([cx, cy])
                        elif label == "Right":
                            right.append([cx, cy])
                    if handshow:  # show the hand data
                        if label == "Left":
                            cv2.putText(img, "left", (left[0][0]-20, left[0][1]),
                                        cv2.FONT_HERSHEY_PLAIN, 1, (0, 0, 255), 2)
                            # cv2.circle(
                            #     img, (left[0][0], left[0][1]), 8, (0, 0, 255), cv2.FILLED)
                        elif label == "Right":
                            cv2.putText(img, "right", (right[0][0]-20, right[0][1]),
                                        cv2.FONT_HERSHEY_PLAIN, 1, (0, 255, 0), 2)
            else:  # if either one hand detected this code will happen
                for i in result.multi_handedness:
                    label = MessageToDict(i)['classification'][0]['label']
                    for handLms in result.multi_hand_landmarks:
                        for id, lm in enumerate(handLms.landmark):
                            h, w, c = img.shape
                            cx, cy = int(lm.x*w), int(lm.y*h)
                            # Id, x, y axis of the point
                            if label == "Left":
                                left.append([cx, cy])
                            elif label == "Right":
                                right.append([cx, cy])
                        # this below line for the 21 dots and connection between them
                        # mpDraw.draw_landmarks(
                        #     img, handLms, mpHande.HAND_CONNECTIONS)
                    if handshow:  # show the hand data
                        if label == "Left":
                            cv2.putText(img, "left", (left[0][0]-20, left[0][1]),
                                        cv2.FONT_HERSHEY_PLAIN, 1, (0, 0, 255), 2)
                        elif label == "Right":
                            cv2.putText(img, "right", (right[0][0]-20, right[0][1]),
                                        cv2.FONT_HERSHEY_PLAIN, 1, (0, 255, 0), 2)

        data = {"status": True, "data": {
            "left": left, "right": right}, "image": img}
        return data

    except KeyboardInterrupt:
        cv2.destroyAllWindows()
        print("\nExit...")
        sys.exit()
        # data = {"status": False, "data": []}
        # return data
