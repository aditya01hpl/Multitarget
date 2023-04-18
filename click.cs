using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Networking;
using Vuforia;

public class click : MonoBehaviour
{
    InputField field;
    InputField Hum;
    public VirtualButtonBehaviour Vb_on;

    void Start()
    {
        field = GameObject.Find("InputFieldTemp").GetComponent<InputField>();

        Hum = GameObject.Find("InputFieldHum").GetComponent<InputField>();

        Vb_on.RegisterOnButtonPressed(OnButtonPressed_on);
        // GameObject.Find("GetButton").GetComponent<Button>().onClick.AddListener(GetData);
    }

    public void OnButtonPressed_on(VirtualButtonBehaviour Vb_on)
    {
        GetData_tem();
        GetData_hum();
        //Debug.Log("Click");
    }

    void GetData_tem() => StartCoroutine(GetData_Coroutine1());
    void GetData_hum() => StartCoroutine(GetData_Coroutine());

    IEnumerator GetData_Coroutine1()
    {
        Debug.Log("Getting Data");
        field.text = "Loading...";
        string uri = "https://aditya-408u.onrender.com/api/v1/user/aditya/temperature";
        using (UnityWebRequest request = UnityWebRequest.Get(uri))
        {
            yield return request.SendWebRequest();
            if (request.isNetworkError || request.isHttpError)
                field.text = request.error;
            else
            {

                field.text = request.downloadHandler.text;
                field.text = field.text.Substring(9, 4);
            }
        }
    }
    IEnumerator GetData_Coroutine()
    {
        Debug.Log("Getting Data");
        Hum.text = "Loading...";
        string uri = "https://aditya-408u.onrender.com/api/v1/user/aditya/humidity";
        using (UnityWebRequest request = UnityWebRequest.Get(uri))
        {
            yield return request.SendWebRequest();
            if (request.isNetworkError || request.isHttpError)
                Hum.text = request.error;
            else
            {

                Hum.text = request.downloadHandler.text;
                Hum.text = Hum.text.Substring(9, 4);
            }
        }
    }
}