package com.example.lab.Entity;

import com.example.lab.Models.AngleModel;
import jakarta.persistence.*;

import java.util.Objects;

@Entity
@Table(name = "convert-Angle")
public class ConvertAngleEntity {
    @Id
    @Column(name = "ID")
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;
    @Column(name = "Angle in degrees")
    private String angleInDegrees;

    @Column(name = "Angle in radians")
    private double angleInRadians;

    public ConvertAngleEntity(int id, String angleInDegrees, double angleInRadians){
        this.id = id;
        this.angleInDegrees = angleInDegrees;
        this.angleInRadians = angleInRadians;
    }

    public ConvertAngleEntity(AngleModel angleModel){
        this.angleInRadians = angleModel.getAngleInRadians();
        this.angleInDegrees = angleModel.getAngleInDegrees();
    }
    public ConvertAngleEntity(){

    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getAngleInDegrees() {
        return angleInDegrees;
    }

    public void setAngleInDegrees(String angleInDegrees) {
        this.angleInDegrees = angleInDegrees;
    }

    public double getAngleInRadians() {
        return angleInRadians;
    }

    public void setAngleInRadians(double angleInRadians) {
        this.angleInRadians = angleInRadians;
    }

    @Override
    public boolean equals(Object o) {
        return ((ConvertAngleEntity) o).id == this.id &&
                this.angleInRadians == ((ConvertAngleEntity) o).angleInRadians &&
                ((ConvertAngleEntity) o).angleInDegrees == this.angleInDegrees &&
                this.getClass() == o.getClass();
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, angleInDegrees, angleInRadians);
    }
}
