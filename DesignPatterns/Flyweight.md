
```
@startuml
skinparam classAttributeIconSize 0

package "class Flyweight(享元模式)" #DDDDDD {
    class FlyweightFactory{
    - mMap:HashMap
    + getFlyweight()
    }
    note right: 享元工厂

    class Flyweight{
    + dosomething()
    }
    note right: 享元对象抽象基类或者接口

    class ConcreteFlyweight{
    - intrinsicState:String
    + dosomething()
    }
    note right: 具体的享元对象

    FlyweightFactory *-- Flyweight
    Flyweight <|-- ConcreteFlyweight
}
@enduml
```
![Flyweight](images/flyweight.png)
