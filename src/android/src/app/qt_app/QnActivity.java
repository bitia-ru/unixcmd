package app.qt_app;

import java.io.File;
import android.os.Build;
import android.os.Bundle;
import android.net.Uri;
import android.content.Intent;
import android.content.res.Configuration;
import org.qtproject.qt6.android.bindings.QtActivity;
import android.view.MotionEvent;

public class QnActivity extends QtActivity
{
    private void deleteFileObject(File fileObject)
    {
        if (fileObject == null)
            return;

        if (fileObject.isDirectory())
        {
            String[] children = fileObject.list();
            for (int i = 0; i < children.length; i++)
                deleteFileObject(new File(fileObject, children[i]));
        }

        fileObject.delete();
    }

    private void clearAppCache()
    {
        try
        {
            deleteFileObject(getCacheDir());
        }
        catch(Exception e)
        {
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // In some cases cache existence leads to the crash after application version upgrade.
        clearAppCache();
    }

    @Override
    public void onNewIntent(Intent intent)
    {
        setIntent(intent);

        super.onNewIntent(intent);
    }
}
