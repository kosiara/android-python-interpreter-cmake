package com.example.bko;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");


    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
    }


    /**
     * Native method call to python initialization.
     *
     * @param androidPrivate
     * @param androidArgument
     * @param serviceEntrypoint
     * @param pythonName
     * @param pythonHome
     * @param pythonPath
     * @param pythonServiceArgument
     * @return
     */
    public static native int nativeStart(
            String androidPrivate, String androidArgument,
            String serviceEntrypoint, String pythonName,
            String pythonHome, String pythonPath,
            String pythonServiceArgument);

    public native String stringFromJNI();
}
