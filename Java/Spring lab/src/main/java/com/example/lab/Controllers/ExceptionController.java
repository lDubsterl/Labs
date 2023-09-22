package com.example.lab.Controllers;

import com.example.lab.Models.ExceptionModel;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;

@RestControllerAdvice
public class ExceptionController {
    Logger logger = LoggerFactory.getLogger(ExceptionController.class);

    @ExceptionHandler(IllegalArgumentException.class)
    public ResponseEntity<ExceptionModel> illegalArgException(IllegalArgumentException exception) {
        var exceptionModel = new ExceptionModel(exception.getMessage(), HttpStatus.BAD_REQUEST);
        logger.error(exceptionModel.getErrorMessage());
        return new ResponseEntity<>(exceptionModel, exceptionModel.getErrorCode());
    }

    @ExceptionHandler(NumberFormatException.class)
    public ResponseEntity<ExceptionModel> notANumberException(NumberFormatException exception) {
        var exceptionModel = new ExceptionModel(exception.getMessage(), HttpStatus.INTERNAL_SERVER_ERROR);
        logger.error(exceptionModel.getErrorMessage());
        return new ResponseEntity<>(exceptionModel, exceptionModel.getErrorCode());
    }
}
