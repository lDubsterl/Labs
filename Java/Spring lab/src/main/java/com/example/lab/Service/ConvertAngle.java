package com.example.lab.Service;

import com.example.lab.Entity.ConvertAngleEntity;
import com.example.lab.Models.AngleCache;
import com.example.lab.Models.AngleModel;
import org.slf4j.Logger;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

import static org.slf4j.LoggerFactory.getLogger;

@Component
public class ConvertAngle {
    @Autowired
    AngleCache<String, Double> cache;
    @Autowired
    RepositoryService repository;
    private AngleModel angleModel;
    Logger logger = getLogger(ConvertAngle.class);

    private Double checkExceptions(String rawAngle) {
        double angleInDegrees;
        try {
            Thread.sleep(100);
        } catch (InterruptedException exception) {
            exception.getMessage();
        }

        try {
            angleInDegrees = Double.parseDouble(rawAngle);
        } catch (NumberFormatException ex) {
            throw new NumberFormatException("Angle must be a number");
        }

        if (Double.isInfinite(angleInDegrees)) {
            throw new IllegalArgumentException("Angle is too big");
        }
        return angleInDegrees;
    }

    public AngleModel checkAndConvert(AngleModel angle) {
        if (angle.getAngleInDegrees() != "Infinity")
            angle.setAngleInDegrees(angle.getAngleInDegrees().replaceAll("[^0-9-.]", ""));
        var convertedAngle = cache.get(angle.getAngleInDegrees());
        if (convertedAngle == null) {
            logger.warn("Cache is empty");

            double angleInDegrees = checkExceptions(angle.getAngleInDegrees());
            double angleInRadians = angleInDegrees * Math.PI / 180;
            angle.setAngleInRadians(angleInRadians);
            cache.push(angle.getAngleInDegrees(), angle.getAngleInRadians());
            if (!repository.angleRepository.existsById(angle.getId()))
                repository.saveToDataBase(new ConvertAngleEntity(angle));
        } else {
            logger.info("got from cache successfully");
            angle.setAngleInRadians(convertedAngle);
        }
        return angle;
    }

    public List<AngleModel> checkAndConvert(List<AngleModel> angle) {
        List<AngleModel> result = new ArrayList<>();
        angle.forEach(input -> result.add(checkAndConvert(input)));
        return result;
    }

    @Async
    public void asyncConvertAngle(AngleModel angle) throws ExecutionException, InterruptedException {
        CompletableFuture.runAsync(() -> {
            try {
                Thread.sleep(10000);
            } catch (InterruptedException e) {
            }

            var model = checkAndConvert(angle);
            ConvertAngleEntity entity = new ConvertAngleEntity();
            entity.setId(model.getId());
            entity.setAngleInDegrees(model.getAngleInDegrees());
            entity.setAngleInRadians(model.getAngleInRadians());
            repository.saveToDataBase(entity);
        });
    }
}
