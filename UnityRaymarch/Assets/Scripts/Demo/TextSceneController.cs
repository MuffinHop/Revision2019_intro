using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
[ExecuteInEditMode]
public class TextSceneController : MonoBehaviour
{
    [SerializeField]
    private TextViewerComponent viewer;

    private List<GameObject> textObjects = new List<GameObject>();
    private bool inited = false;
    private int currentText = 0;
    void Awake()
    {
        int textIndex = 1;
        foreach (var sceneName in viewer.textSceneNames)
        {
            StartCoroutine(LoadYourAsyncScene(sceneName, textIndex));
            textIndex++;
        }
    }

    IEnumerator LoadYourAsyncScene(string name, int index)
    {
        AsyncOperation asyncLoad = SceneManager.LoadSceneAsync(name, LoadSceneMode.Additive);

        while (!asyncLoad.isDone)
        {
            yield return null;
        }

        Scene textScene = SceneManager.GetSceneByName(name);

        List<GameObject> rootObjects = new List<GameObject>();
        textScene.GetRootGameObjects(rootObjects);
        for (int i = 0; i < rootObjects.Count; ++i)
        {
            GameObject gameObject = rootObjects[i];
            gameObject.SetActive(false);
            textObjects.Add(gameObject);
        }
    }

    // Update is called once per frame
    void Update()
    {
        var sync = (int)SyncUp.GetVal("TextId");

        if ((sync - 1) >= textObjects.Count) return;

        if (sync != currentText && currentText > 0)
        {
            textObjects[currentText - 1].SetActive(false);
        }
        if (sync == 0) { currentText = 0; }

        if (sync > 0 && sync != currentText)
        {
            currentText = sync;

            textObjects[currentText - 1].SetActive(true);
        }
    }
}
