package com.example.lab.Models;

import java.util.List;

public class ExtendedAngle {

    private List<AngleModel> angle;
    private double min;
    private double max;
    private double average;

    public ExtendedAngle(){};
    public List<AngleModel> getAngle() {
        return angle;
    }

    public void setAngle(List<AngleModel> angle) {
        this.angle = angle;
    }

    public double getMax() {
        return max;
    }

    public void setMax(double max) {
        this.max = max;
    }

    public double getMin() {
        return min;
    }

    public void setMin(double min) {
        this.min = min;
    }
    public double getAverage() {
        return average;
    }

    public void setAverage(double average) {
        this.average = average;
    }
}
