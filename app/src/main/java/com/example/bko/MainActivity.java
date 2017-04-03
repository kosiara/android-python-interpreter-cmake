package com.example.bko;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import java.io.File;

/**
 * Runs basic Python 3.5 interpreter on Android.
 * Runs a basic string script.
 * Runs script from assets/python/main.py file.
 *
 * @author bkosarzycki
 */
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
        String jniString = stringFromJNI();

        // Extracts assets from apk file
        AssetExtractor assetExtractor = new AssetExtractor(this);
        assetExtractor.removeAssets("python");
        assetExtractor.copyAssets("python");
        String appDataDir = assetExtractor.getAppDataDir();

        //initialize python
        initializePython(appDataDir);

        tv.setText(jniString);
    }

    private void initializePython(String appDataDir) {

        String androidArgument = appDataDir;
        String serviceEntrypoint = "assets/python/main.py";

        String app_root =  getFilesDir().getAbsolutePath() + "/app";
        File app_root_file = new File(app_root);
        PythonUtil.loadLibraries(app_root_file);

        // === Check whether assets have been extracted ===================
        if (!new File(appDataDir + "/assets/python/stdlib.zip").exists())
            throw new RuntimeException("Assets haven't been extracted !");

        nativePythonStart(androidArgument, serviceEntrypoint);
    }

    /**
     * Native method call to python initialization.
     *
     * @param androidArgument
     * @param serviceEntrypoint
     * @return
     */
    public static native int nativePythonStart(String androidArgument, String serviceEntrypoint);

    public native String stringFromJNI();
}
