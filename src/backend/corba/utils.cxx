//
// Created by ingvord on 10/13/16.
//

namespace Tango {
    /*static*/void setClientConnectTimeout(DevULong timeout){
        omniORB::setClientConnectTimeout(timeout);
    }
}//tango