package com.example.lab.Repository;

import com.example.lab.Entity.ConvertAngleEntity;
import org.springframework.data.repository.CrudRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface AngleRepository extends CrudRepository<ConvertAngleEntity, Integer> {

}
