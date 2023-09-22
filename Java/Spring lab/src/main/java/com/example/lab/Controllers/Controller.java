package com.example.lab.Controllers;

import com.example.lab.Entity.ConvertAngleEntity;
import com.example.lab.Models.AngleModel;
import com.example.lab.Models.ExtendedAngle;
import com.example.lab.Repository.AngleRepository;
import com.example.lab.Service.ConvertAngle;
import com.example.lab.Service.CounterService;
import com.example.lab.Service.StatisticsService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.scheduling.annotation.Async;
import org.springframework.web.bind.annotation.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.List;
import java.util.concurrent.ExecutionException;

@RestController
public class Controller {
    Logger logger = LoggerFactory.getLogger(Controller.class);
    @Autowired
    ConvertAngle convertingService;
    @Autowired
    StatisticsService statConvertingService;
    @Autowired
    CounterService counter;

    @Autowired
    AngleRepository repository;

    @GetMapping(value = "/convertAngle")
    public ResponseEntity<?> degreesToRadians(@RequestParam String angleInDegrees) {
        var model = new AngleModel();
        model.setAngleInDegrees(angleInDegrees);
        var convertedAngle = convertingService.checkAndConvert(model);
        counter.increment();
        logger.info("converted successfully");
        return new ResponseEntity<Object>(convertedAngle, HttpStatus.OK);
    }
    @GetMapping("/counter")
    public int getCounter(){
        return counter.getCounter();
    }
    @PostMapping("/convertAngle")
    public List<AngleModel> degreesToRadians(@RequestBody List<AngleModel> angles)
    {
        var result = convertingService.checkAndConvert(angles);
        for (int i = 0; i < angles.size(); i++)
            counter.increment();
        logger.info("converted successfully");
        return result;
    }
    @PostMapping("/convertWithStatistics")
    public ExtendedAngle degreesToRadiansWithStatistics(@RequestBody List<AngleModel> angles)
    {
        var angle = convertingService.checkAndConvert(angles);
        ExtendedAngle result = statConvertingService.countStatistics(angle);
        for (int i = 0; i < angles.size(); i++)
            counter.increment();
        logger.info("converted successfully");
        return result;
    }
    @Async
    @PostMapping("/convert-angle-async")
    public ResponseEntity<String> asyncConvertAngle(String angleInDegrees) throws InterruptedException, ExecutionException {
        var model = new AngleModel();
        model.setAngleInDegrees(angleInDegrees);
        var emptyEntity = new ConvertAngleEntity();
        model.setId(repository.save(emptyEntity).getId());
        convertingService.asyncConvertAngle(model);
        return new ResponseEntity<>("Request was sent! id: " + Integer.toString(model.getId()), HttpStatus.OK);
    }
}
