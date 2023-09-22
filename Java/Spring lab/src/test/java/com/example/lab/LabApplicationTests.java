package com.example.lab;

import com.example.lab.Models.AngleCache;
import com.example.lab.Models.AngleModel;
import com.example.lab.Service.ConvertAngle;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mockito;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.junit.jupiter.SpringExtension;
import static org.junit.jupiter.api.Assertions.*;
@ExtendWith(SpringExtension.class)
@SpringBootTest
class LabApplicationTests {
    @Autowired
    ConvertAngle convertAngle;
    @Autowired
    AngleCache<String, Double> cache;
    @Test
    void error_status_500() {
        var model = new AngleModel();
        model.setAngleInDegrees("AMOGUS");
        Exception exc = assertThrows(NumberFormatException.class, () -> {
            convertAngle.checkAndConvert(model);
        });
        String expectedMessage = "Angle must be a number";
        String actualMessage = exc.getMessage();
        assertTrue(actualMessage.contains(expectedMessage));
    }

    @Test
    void error_status_400() {
        var model = new AngleModel();
        model.setAngleInDegrees(Double.toString(Double.MAX_VALUE * 2));
        Exception exc = assertThrows(IllegalArgumentException.class, () -> {
            convertAngle.checkAndConvert(model);
        });
        String expectedMessage = "Angle is too big";
        String actualMessage = exc.getMessage();
        assertTrue(actualMessage.contains(expectedMessage));
    }

    @Test
    void ok_status_200() {
        var model = new AngleModel();
        model.setAngleInDegrees(Double.toString(Mockito.anyDouble()));
        var result = convertAngle.checkAndConvert(model);
        assertEquals(Mockito.anyDouble(), result.getAngleInRadians());
    }

}
