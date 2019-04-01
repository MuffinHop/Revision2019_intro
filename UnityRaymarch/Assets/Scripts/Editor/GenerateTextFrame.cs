using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEditor;

public class GenerateTextFrame
{
    [MenuItem("DemoEngine/Clear PlayerPrefs")]
    private static void ClearPlayerPrefs()
    {
        PlayerPrefs.DeleteAll();
    }

    [MenuItem("DemoEngine/Generate Text Frame Code")]
    private static void GenerateTextFrameCode()
    {
        var logOutput = "";

        logOutput+=("// --------------------------------------------- START START\n");
        GameObject canvas = Selection.activeTransform.gameObject;
        // 	headingFont = CreateFont(YRES*0.2, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Impact");

        List<string> fontNames = new List<string>();

        foreach (Transform child in canvas.transform)
        {
            var t = child.gameObject.GetComponent<Text>();
            var text = t.text;
            var fontName = t.font.name + t.fontSize;
            if (!fontNames.Contains(fontName))
            {
                fontNames.Add(fontName);
                logOutput += "HFONT " + fontName + "Font = NULL;\n";
            }
        }

        logOutput += "/////////////// CREATE FONTS\n";

        List<string> fontDefined = new List<string>();

        foreach (Transform child in canvas.transform)
        {
            var t = child.gameObject.GetComponent<Text>();
            var text = t.text;
            var fontName = t.font.name + t.fontSize;
            if (!fontDefined.Contains(fontName))
            {
                logOutput += fontName + "Font = CreateFont(" + t.fontSize + ", 0,0,0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, \"" + t.font.name + "\");\n";
                fontDefined.Add(fontName);
            }
        }

        logOutput += "/////////////// DRAW\n";
        logOutput += "// canvas: " + canvas.name + "\n";

        foreach (Transform child in canvas.transform)
        {
            var t = child.gameObject.GetComponent<Text>();
            var text = t.text;
            var fontName = t.font.name + t.fontSize;
            var tt = child.gameObject.GetComponent<RectTransform>();

            Vector3[] v = new Vector3[4];
            tt.GetWorldCorners(v);

            Vector3 pt1 = RectTransformUtility.WorldToScreenPoint(null, v[0]);
            Vector3 pt2 = RectTransformUtility.WorldToScreenPoint(null, v[1]);
            Vector3 pt3 = RectTransformUtility.WorldToScreenPoint(null, v[2]);
            Vector3 pt4 = RectTransformUtility.WorldToScreenPoint(null, v[3]);

            var left = (int)pt1.x;
            var top = (1080-(int)pt2.y);
            var right = (int)pt3.x;
            var bottom = (1080-(int)pt1.y);
            logOutput += "hFontOld = SelectObject(fonthDC, " + fontName + "Font);\n";
            logOutput += "DrawRectText(\"" + text + "\",RGB(" + (int)(t.color.r * 255) + "," + (int)(t.color.g * 255) + "," + (int)(t.color.b * 255) + "),RGB(" + (int)(Camera.main.backgroundColor.r * 255) + "," + (int)(Camera.main.backgroundColor.g * 255) + "," + (int)(Camera.main.backgroundColor.b*255) + ")," + left + "," + top + "," + bottom + "," + right + ");\n";
           }



        /*
   	SetTextColor(fonthDC, RGB(255, 0, 0));
	SetBkColor(fonthDC, 0x0000000);
	SetTextAlign(fonthDC, TA_TOP);

	RECT rc1;
	rc1.top = 0;
	rc1.left = 0;
	rc1.bottom = YRES / 8;
	rc1.right = XRES;

	FontInRect(cap1_s, rc1);
         * */

        logOutput+=("// --------------------------------------------- END END END\n");
        Debug.Log(logOutput);
    }
}
