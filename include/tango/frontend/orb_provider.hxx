//
// Created by ingvord on 10/24/16.
//

#pragma once

#include <tango/frontend/orb.hxx>

namespace Tango {
    class TangORBProvider {
    public:
        template <typename T>
        TangORBProvider(T&& orb):orb_{std::forward<T>(orb)}{};
        virtual ~TangORBProvider(){};
    public:
        //TODO return weak ptr
        TangORB_var get(){
            return orb_;
        };
        void destroy(){
            orb_.get()->destroy();
        }
    protected:
        TangORB_var orb_;
    };

    using TangORBProvider_var = std::shared_ptr<TangORBProvider>;
}//Tango