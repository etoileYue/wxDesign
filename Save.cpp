#include "Save.hpp"


string SaveJson(vector<Sign> SelectedSigns, vector<string> sthelse) {
        Value root;
        Value design;
        Value components;
        Value lib;
        Value nets;

        Value Sign;
        Value pos;
        for (auto& sign : SelectedSigns) {
            Sign["id"] = sign.getID();
            Sign["name"] = sign.getName();
            pos.append(sign.getPos(0));
            pos.append(sign.getPos(1));
            components.append(Sign);

            Value net;
            Value pinsConnected;
            net["name"] = sign.getName();
            for (auto& connect : sign.connected) {
                string pinnum = "pin" + to_string(connect.pinId);
                string otherpin = "pin" + to_string(connect.pin.pinId);
                Value pin;
                pin.append(connect.Signname);
                pin.append(otherpin);
                pinsConnected[pinnum] = pin;
            }
            net["connected"] = pinsConnected;
            nets.append(net);

        }

        design["source"] = sthelse[0];
        design["date"] = sthelse[1];
        design["title"] = sthelse[2];

        root["design"] = design;
        root["components"] = components;
        root["lib"] = sthelse[3];
        root["nets"] = nets;

        string str = root.toStyledString();
        
        return str;
    };