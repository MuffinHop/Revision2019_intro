using UnityEngine;

public class Screenshotter : MonoBehaviour
{
    void Start()
    {
        ScreenCapture.CaptureScreenshot("capture.png", 4);
    }
}