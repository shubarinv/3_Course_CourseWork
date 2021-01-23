//
// Created by Vladimir Shubarin on 22.01.2021.
//

#ifndef CGCOURSEWORK_CAR_MANAGER_HPP
#define CGCOURSEWORK_CAR_MANAGER_HPP

#include "mesh.hpp"
#include <random>
#include <utility>

class CarManager {

public:
    struct Path {
        Path(std::vector<glm::vec3> _path,bool _circularised,
             float _anglePerTick,
             float _initialRot){
            path=std::move(_path);
            circularised=_circularised;
            anglePerTick=_anglePerTick;
            initialRot=_initialRot;
        }
        std::vector<glm::vec3> path;
        bool circularised = true;
        float anglePerTick = 0;
        float initialRot = 0;
    };
    struct Car {
        Mesh *mesh;
        Path *path;
        int posOnPath = 0;
        float rotation = 0;
    };
private:
    template<typename Numeric, typename Generator = std::mt19937>
    Numeric random(Numeric from, Numeric to) {
        thread_local static Generator gen(std::random_device{}());

        using dist_type = typename std::conditional<
                std::is_integral<Numeric>::value, std::uniform_int_distribution<Numeric>,
                std::uniform_real_distribution<Numeric> >::type;

        thread_local static dist_type dist;

        return dist(gen, typename dist_type::param_type{from, to});
    }

    int carsOnMap{0};
    int maxCarsOnMap{10};
    std::vector<Car> cars;
    std::vector<ObjLoader::loadedOBJ>car1;
    std::vector<ObjLoader::loadedOBJ>car2;
public:
    CarManager(){
        ObjLoader objLoader;
        car1=objLoader.loadObj("../resources/models/car_1.fbx");
        car2=objLoader.loadObj("../resources/models/car_2.fbx");
    }
    void addPath(std::vector<glm::vec3> path, bool circularised = true, float anglePerTick = 0, float initialRot = 0) {
        paths.push_back(new Path(std::move(path), circularised, anglePerTick, initialRot));
    }

    void addCar() {
        if (!paths.empty()) {
            Car car;
            car.mesh = new Mesh(car1);
            car.path = paths[random<int>(0, paths.size() - 1)];
            car.mesh->addTexture("../textures/Car Texture 1.png")->compile();
            car.posOnPath = random<int>(0, car.path->path.size() - 1);
            car.mesh->setPosition(car.path->path[car.posOnPath]);
            car.rotation=car.path->initialRot+(car.path->anglePerTick*(float)car.posOnPath);
            updateCarPosOnPath(&car);
            cars.push_back(car);
        }
    }

    static float findRotation(Car *car) {
        return car->rotation+= car->path->anglePerTick;
    }

    void draw(Shader *shader) {
        for (auto &car:cars) {
            if (car.mesh != nullptr) {
                car.mesh->draw(shader);
                updateCarPosOnPath(&car);
            }
        }
    }

    static void updateCarPosOnPath(Car *car) {
        car->mesh->setPosition(car->path->path[car->posOnPath])->setOrigin(car->path->path[car->posOnPath])->setRotation({0,findRotation(car),0});
        car->posOnPath++;
        if (car->posOnPath >= car->path->path.size()) {
            if (car->path->circularised) {
                car->posOnPath = 0;
                car->rotation=car->path->initialRot;
            } else {
                car->mesh = nullptr;
            }
        }
    }

    std::vector<Path *> paths;


};


#endif //CGCOURSEWORK_CAR_MANAGER_HPP
