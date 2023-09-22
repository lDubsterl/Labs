package com.example.lab.Models;

import com.fasterxml.jackson.annotation.JsonIgnore;

public class AngleModel {

    private String angleInDegrees;

    private double angleInRadians;

    public void setId(int id) {
        this.id = id;
    }

    @JsonIgnore
    public int getId() {
        return id;
    }

    private int id;

    public AngleModel(String angleInDegrees) {
        this.angleInDegrees = angleInDegrees;
    }

    public AngleModel() {
    }

    public String getAngleInDegrees() {
        return angleInDegrees;
    }

    public double getAngleInRadians() {
        return angleInRadians;
    }

    public void setAngleInDegrees(String angleInDegrees) {
        this.angleInDegrees = angleInDegrees;
    }

    public void setAngleInRadians(double angleInRadians) {
        this.angleInRadians = angleInRadians;
    }
}
