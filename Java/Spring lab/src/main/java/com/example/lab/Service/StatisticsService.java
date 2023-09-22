package com.example.lab.Service;

import com.example.lab.Models.AngleModel;
import com.example.lab.Models.ExtendedAngle;
import org.springframework.stereotype.Component;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

@Component
public class StatisticsService {
    public ExtendedAngle countStatistics(List<AngleModel> angles) {
        var stat = new ExtendedAngle();
        stat.setAngle(angles);
        stat.setMax(countMax(angles));
        stat.setMin(countMin(angles));
        stat.setAverage(countAverage(angles));
        return stat;
    }

    private List<Double> modelToNumbers(List<AngleModel> angles) {
        var numbers = angles.stream().map(angle -> {
            double num;
            num = Double.parseDouble(angle.getAngleInDegrees());
            return num;
        }).collect(Collectors.toList());
        return numbers;
    }

    public double countMax(List<AngleModel> angles) {
        return Collections.max(modelToNumbers(angles));
    }

    public double countMin(List<AngleModel> angles) {
        return Collections.min(modelToNumbers(angles));
    }

    public double countAverage(List<AngleModel> angles) {
        double average = 0;
        var numberArray = modelToNumbers(angles);
        for (int i = 0; i < numberArray.size(); i++)
            average += numberArray.get(i);
        average /= numberArray.size();
        return average;
    }
}
