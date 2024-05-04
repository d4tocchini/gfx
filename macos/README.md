
# gfx/macos

> implementation notes

https://github.com/yue/yue/tree/master/nativeui/mac
https://github.com/yue/yue/tree/master/nativeui/gfx/mac

## obj-c

tranlsate obj-c to c:
```sh
    clang -rewrite-objc main.m
```

* [NSView vs. NSView?](https://developer.apple.com/swift/blog/?id=25)

## ARC

* [ARC guide](https://developpaper.com/objective-c-memory-management-2-from-mrc-to-arc/)
* [clang ARC ref](https://clang.llvm.org/docs/AutomaticReferenceCounting.html)

ARC variable modifiers:
* __strong
* __weak
* __unsafe_unretaied
* __autoreleasing

Methods in the alloc, copy, init, mutableCopy, and new families are implicitly marked:
```c
    __attribute__((ns_returns_retained))
```

[When to use __bridge, __bridge_transfer/CFBridgingRelease and __bridge_retained/CFBridgingRetain](https://medium.com/@hadhi631/when-to-use-bridge-bridge-transfer-cfbridgingrelease-and-bridge-retained-cfbridgingretain-4b3d2fc932df)
    * __bridge (CoreFoundation ← → Foundation)
    * __bridge_transfer / CFBridgingRelease (CoreFoundation → Foundation)
    * __bridge_retained / CFBridgingRetained (Foundation → CoreFoundation)


[ARC: __bridge versus __bridge_retained using contextInfo test case](https://stackoverflow.com/questions/20842310/arc-bridge-versus-bridge-retained-using-contextinfo-test-case)

```
// Object to void *:
contextInfo:(__bridge void *)s

// void * to object:
NSString *s = (__bridge NSString *)context;

// or...

// Object to void *, retaining the object:
contextInfo:(__bridge_retained void *)s

// void * to object, transferring ownership.
// The object is released when s goes out of scope:
NSString *s = (__bridge_transfer NSString *)context;
```

## how-to

[count the number of all subviews on a view?](https://developpaper.com/algorithm-count-the-number-of-all-subviews-on-a-view/)

[SDL2 (fork) SDL_CreateWindowFrom pass NSView under mac osx](https://github.com/flowercodec/sdl2/commit/616e6c429a037ef50ab9d0e454771a643f52e3fb)
