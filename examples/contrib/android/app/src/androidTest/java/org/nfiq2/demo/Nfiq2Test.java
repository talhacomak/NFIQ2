package org.nfiq2.demo;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.core.app.ActivityScenario;
import androidx.test.platform.app.InstrumentationRegistry;
import android.content.Context;
import android.widget.CheckBox;
import gov.nist.nfiq2.Nfiq2;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.junit.Assert.*;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

/** Run on an ARM64 device using connectedDebugAndroidTest. */
@RunWith(AndroidJUnit4.class)
public class Nfiq2Test {
    private static final String MODEL_ASSET = "nist_plain_tir-ink.yaml";
    private static final String MODEL_HASH = "b4a1e7586b3be906f9770e4b77768038";

    @BeforeClass public static void initializeNfiq2() {
        Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();
        try {
            Nfiq2.initialize(context.getAssets(), MODEL_ASSET,
                "00000000000000000000000000000000");
            fail("Expected model hash validation");
        } catch (IllegalStateException expected) { }
        Nfiq2.initialize(context.getAssets(), MODEL_ASSET, MODEL_HASH);
    }

    @Test public void ppiConfirmationSurvivesActivityRecreation() {
        Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();
        context.getSharedPreferences("fingerprint_quality_preferences", Context.MODE_PRIVATE)
            .edit().clear().commit();
        try (ActivityScenario<MainActivity> scenario = ActivityScenario.launch(MainActivity.class)) {
            scenario.onActivity(activity -> {
                CheckBox confirmation = activity.findViewById(R.id.confirm);
                assertFalse(confirmation.isChecked());
                confirmation.setChecked(true);
            });
            scenario.recreate();
            scenario.onActivity(activity ->
                assertTrue(((CheckBox) activity.findViewById(R.id.confirm)).isChecked()));
        } finally {
            context.getSharedPreferences("fingerprint_quality_preferences", Context.MODE_PRIVATE)
                .edit().clear().commit();
        }
    }

    @Test public void referenceFingerprintAndRepeatedCall() throws Exception {
        byte[] data;
        try (InputStream input = InstrumentationRegistry.getInstrumentation().getContext().getAssets().open("SFinGe_Test01.pgm")) {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            byte[] buffer = new byte[8192];
            int read;
            while ((read = input.read(buffer)) != -1) output.write(buffer, 0, read);
            data = output.toByteArray();
        }
        // This repository fixture has a fixed P5 header, no comments or metadata.
        byte[] header = "P5\n416 560\n255\n".getBytes(StandardCharsets.US_ASCII);
        assertEquals(header.length + 416 * 560, data.length);
        for (int i = 0; i < header.length; i++) assertEquals(header[i], data[i]);
        byte[] pixels = new byte[416 * 560];
        System.arraycopy(data, header.length, pixels, 0, pixels.length);
        assertEquals(54, Nfiq2.score(pixels, 416, 560, 500));
        assertEquals(54, Nfiq2.score(pixels, 416, 560, 500));
    }
    @Test public void invalidBufferRejectedBeforeNativeRead() {
        try {
            Nfiq2.score(new byte[1], 416, 560, 500);
            fail("Expected input validation");
        } catch (IllegalArgumentException expected) { }
    }
    @Test public void wrongPpiRejected() {
        try {
            Nfiq2.score(new byte[32 * 32], 32, 32, 1000);
            fail("Expected PPI validation");
        } catch (IllegalArgumentException expected) { }
    }
}
