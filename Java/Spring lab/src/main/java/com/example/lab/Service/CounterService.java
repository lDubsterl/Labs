package com.example.lab.Service;

import org.springframework.stereotype.Component;

@Component
public class CounterService {
    private int counter;
    public int getCounter(){
        return counter;
    }
    public synchronized void increment(){
        counter++;
    }
}
