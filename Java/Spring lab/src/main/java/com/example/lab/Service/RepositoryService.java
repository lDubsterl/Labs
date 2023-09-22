package com.example.lab.Service;

import com.example.lab.Entity.ConvertAngleEntity;
import com.example.lab.Repository.AngleRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.ArrayList;

@Component
public class RepositoryService {
    @Autowired
    AngleRepository angleRepository;

    public void saveToDataBase(ConvertAngleEntity dataEntity) {
        if (!containsSuchEntity(dataEntity)) {
            angleRepository.save(dataEntity);
        }
    }

    private boolean containsSuchEntity(ConvertAngleEntity currentEntity) {
        ArrayList<ConvertAngleEntity> entities = new ArrayList<>();
        angleRepository.findAll().forEach(entities::add);
        return entities.contains(currentEntity);
    }

    public ArrayList<ConvertAngleEntity> getAllData() {
        ArrayList<ConvertAngleEntity> entities = new ArrayList<>();
        angleRepository.findAll().forEach(entities::add);
        return entities;
    }

    public ConvertAngleEntity getEntityById(Integer id) {
        return angleRepository.findById(id).orElse(null);
    }
}
