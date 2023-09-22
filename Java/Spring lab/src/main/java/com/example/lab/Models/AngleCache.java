package com.example.lab.Models;

import org.springframework.stereotype.Component;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

@Component
public class AngleCache<Key, Data> {
    private final Map<Key, Data> cache = Collections.synchronizedMap(new HashMap<>());
    public boolean push(Key key, Data value) {
        if (!cache.containsKey(key)) {
            cache.put(key, value);
            return true;
        }
        return false;
    }

    public Data get(Key key) {
        if (cache.containsKey(key))
            return cache.get(key);
        return null;
    }
}
