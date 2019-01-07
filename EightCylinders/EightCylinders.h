// EightCylinders.h

#pragma once

using namespace System;

#include <v8.h>


namespace EightCylinders {

    public ref class Class1
    {
    public:
        void Test() 
        {
            v8::Isolate::CreateParams params;
            v8::Isolate* isolate = v8::Isolate::New(params);
        }
        // TODO: Add your methods for this class here.
    };
}
