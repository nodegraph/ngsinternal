Dealing with Complexity
-----------------------
One of the main obstacles for software development is complexity. Increasing complexity has the following negative effets on software: 

* increased number of bugs
* increased maintenance efforts
* fewer and fewer people want to understand it, and work on it

Depedency Oriented Programming (DOP)
------------------------------------

Dependency Oriented Programming is the name chosen to describe this new style of programming, where dependencies between components are explicitly written out in code.

What is DOP?
------------
DOP is modeled after node graphs. Node graphs are visually able to represent any type of compute, and have a very well known set of rules in terms of how they operate. Let's call this the Node Graph Paradigm.

* Nodes have parameters and links that configure what they compute.
* Links are a visual way to specify a node's dependencies on other nodes.
* Nodes perform their compute when their input links are clean.
* Parameter changes on nodes propagate dirtiness downstream to other nodes via links.

Dependency Oriented Programming is basically just bringing the Node Graph Paradigm right into realm of C++ objects. Our DOP framework allows c++ object instances to explicitly track and depend on other object instances. The framework prevents cyclic dependencies.

Our DOP Object Model
--------------------

* An "Entity" is used to represent things (like nodes or links) at a fairly abstract level. It is very similar to the concept of "Entity" used in game development. Eg. A tank, a player, non-player character (NPC) can all be considered entities.
* Entities are just bags that contain "Components". Entities can have child entities too. All entities have names and a path starting at the root entity.
* Components form the essence of the Entity.  New Entities can be created easily by mixing and matching any number of components. Components are like plug and play hardware for our Entities. Components can even be changed at runtime.
* Components explicitly state their dependency on other Components held by it's own encompassing Entity or other Entities. They use a special Dep<Compoment> object to express this.
* The Dep<Component> acts like a std::shared_ptr. As long as you hold onto it the dependency between you and other Component will be tracked. Whenever you call methods on a Component through the Dep<Component>, dirtiness will be propopaged to all dependedant components. Also circular dependencies are not allowed. This prevents spaghetti code, leading to a clear dependency propagation oriented flow of execution.
* You can clean components simply by calling "clean()" on it. The framework uses the currently held Dep<Component> pointers to determine which components need to be cleaned and in what order they should be cleaned.


Ownership vs Dependency
-----------------------

* In many programming or object models, object ownership/composition is intimately tied to actually using or dependeing on the owned/composed object. This often leads to methods similar to forwarding methods, which try not to expose that composed object to the outside world. In many cases the composed object will even have a back pointer to the composing object, leading to a circular pointer based dependency.
* Continuing in this fashion can lead to spaghetti code where calls occur back and forth across these pointers between objects. It becomes very unclear when an object is up to date or when it needs to be refreshed. Also how to update or refresh an object becomes very unclear.
* In our DOP object model object ownership is decoupled from actually depending on a object. The Entity is simply bag which owns its components, simply for the purposes of allocating and deallocating its components all at once.
* The Components in an Entity can dependend on any other Component owned by any other Entity. 
Entities don't do anything except create an owner ship hierarchy of other entities and components, making them accessible via a string based path.


Signals vs Dirty Propagation
----------------------------
* Our system propagates dirtiness to components automatically. Components simply have an update method which is guaranteed to get called when all its dependencies are clean.
* The benefit of this approach is that we don't get into a situations where there are lots of differnt signals, signalling to arbitrary locations in the code base. This is similar to spaghetti code, but for signals.
* However the downside is that the dirty state alone doesn't provide information on what specifically changed in its dependencies. If the Component knew this it could possibly update more efficiently.
* In practice with appropriate dop design this cost is negligible.

The Code
--------
* This system is implemented in the "nodegraphsoftware/src/base/objectmodel" folder.

Interfacing with External Systems
---------------------------------
* We strictly prohibit the use of signals inside our DOP based system, because all the signal and callback mechanisms can all be accounted for cleanly with our dirty dependency propagation model for objects.
* However at the gui layer we use all the popular signal and callback mechanisms, because it is so embedded into these systems.
* The exact location where the dop dirty system switchs into the qt signalling system is when dirtiness is propagated to a gui based Component. At the time dirtiness is propagated to it, it will call Qt's update method method.
* In the opposite direction when a user takes some action in a gui based Component, it will ultimately end up maniupulating other components including itself thereby propagating dirtiness throughout the dop system.
