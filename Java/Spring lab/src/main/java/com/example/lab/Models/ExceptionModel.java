package com.example.lab.Models;

import org.springframework.http.HttpStatus;

public class ExceptionModel {
    private HttpStatus errorCode;

    private String errorMessage;

    public ExceptionModel(String errorMessage, HttpStatus errorCode) {
        this.errorCode = errorCode;
        this.errorMessage = errorMessage;
    }

    public HttpStatus getErrorCode() {
        return errorCode;
    }

    public String getErrorMessage() {
        return errorMessage;
    }
}
