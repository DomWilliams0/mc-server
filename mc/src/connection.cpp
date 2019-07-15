#include "connection.h"
#include "packet.h"
#include "loguru.hpp"
#include "util.h"

#include <typeinfo>
#include <sstream>
#include <cassert>
#include <cstring>
#include <memory>

mc::BaseConnection::BaseConnection(const mc::Socket &socket) : socket(socket) {}


mc::BaseConnection *mc::BaseConnection::handle_packet(mc::Buffer &packet) {
    // parse header
    Varint packet_id;
    packet.read(packet_id);

    // match packet type
    // TODO match_packet is leaked if anything errors
    BasePacket *concrete_packet = match_packet(*packet_id, packet); // TODO is packet needed here?
    if (concrete_packet == nullptr) {
        std::ostringstream ss;
        ss << "resolving packet id " << *packet_id << " in connection '" << type_name() << "'";
        throw Exception(ErrorType::kUnexpectedPacketId, ss.str());
    }

    // populate packet fields
    concrete_packet->read_body(packet);
    DLOG_F(INFO, "packet: %s", concrete_packet->to_string().c_str());

    // handle packet in connection, maybe generating a response and a new state
    mc::BasePacket *response = nullptr;
    BaseConnection *new_connection = handle_packet(concrete_packet, &response);

    // send response if necessary
    if (response != nullptr) {
        DLOG_F(INFO, "sending response: %s", response->to_string().c_str());

        mc::Buffer buffer;
        response->write(buffer);
        socket.write(buffer);
    }

    assert(new_connection != nullptr);
    return new_connection;
}

mc::BaseConnection::BaseConnection(const mc::BaseConnection &other) : socket(other.socket) {

}

mc::ConnectionHandshaking::ConnectionHandshaking(const mc::Socket &socket) : BaseConnection(socket) {}

mc::BasePacket *mc::ConnectionHandshaking::match_packet(mc::Varint::Int packet_id, mc::Buffer &packet) {
    UNUSED(packet);
    switch (packet_id) {
        case 0x00:
            return new PacketHandshake;
        default:
            return nullptr;
    }
}

mc::BaseConnection *mc::ConnectionHandshaking::handle_packet(mc::BasePacket *packet, BasePacket **response) {
    auto *handshake = dynamic_cast<PacketHandshake *>(packet);
    UNUSED(response);

    if (*handshake->next_state == 1) {
        // status
        return new ConnectionStatus(*this);
    }

    if (*handshake->next_state == 2) {
        // login
        return new ConnectionLogin(*this);
    }

    throw Exception(ErrorType::kBadEnum, "bad next_state");
}

mc::BasePacket *mc::ConnectionStatus::match_packet(mc::Varint::Int packet_id, mc::Buffer &packet) {
    UNUSED(packet);

    switch (packet_id) {
        case 0x00:
            return new PacketEmpty;
        case 0x01:
            return new PacketPing;
        default:
            return nullptr;
    }
}

mc::BaseConnection *mc::ConnectionStatus::handle_packet(mc::BasePacket *packet, BasePacket **response) {
    if (*packet->packet_id == 0x00) {

        char json[] = "{\"description\":{\"text\":\"My Nice Server\"},\"players\":{\"max\":800,\"online\":409},"
                      "\"version\":{\"name\":\"1.14.3\",\"protocol\":490}, \"favicon\": \"data:image/png;base64,"
                      "iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA"
                      "6AAAdTAAAOpgAAA6mAAAF3CculE8AAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAS10lEQVRo3q16"
                      "S49kyXXed86JiPvIV1X1c3oeMkHCI8l6wBzbC3NlGga80U6AoaW2hn+Bf4sW2theyRYg2DJgGBZBS7ABQ6TlsUhpOBwOOdM9"
                      "7O6qrqzKvK+IOMeLm3kru6ure3o8gUIi7628cc8X5/1FkJnh5aMHYIBCx18o1GCFCcAAYAzQ/gsg+YZ5sH+cABjYQBmWwQQw"
                      "wAYxiAEKEODwRoNuBKD7N+f9FwKAGKavN8p6OHwP0P7v8EnRK/BXzxv4prnfEMDGwAQHMCAG0h2kHF6U2BQA/A3v7Q/uK8zM"
                      "kBVA3Q5ghgOEIQyy/Sr5r0kD6J+/3C+VpavVwsH65Xgw65XUnfMMJRDBCGDo7ic6u1IIKQC1BIDp+RX66gA6BQDN0AEwOIYX"
                      "CJsSER2IuBMog186jaQJeb6yMKLOMwA2ELDzKgMMkDeS/2YAMYEY4J0LjB6Rgdm49OMnwWx3qfsXv2DqYfyvYvRxA0b4Sleu"
                      "IQqirwjgRp9vHQAw1CF55BybfnsRu6a/6M1MNeWcs0ZVVU1mNloQEY2ftFdRnAUiEee5KEIxC2UlRelcKGC7FSIGWBVZoYpC"
                      "vi4fuPh5enZ2/vDz7vQJNxeua6lrLPa9bwHsLGfn2iAGbfML0u8+xWUzg0tgk2DiwQHC63CvmM3qo+PZnTvVrdt+tWJfZ1B4"
                      "wzh6I4CzH39v/V/+/WxFZzxf9c7z5TYoxYWhGx8hImZmZgBmJge6t4PhvZ9uAlDV3ePantPieD1c0FnA0Tf+5b9O+ciXlrOJ"
                      "7KZKKRHRdPlmJjQKR7Rb0WkwaBfviHhvKNOd6fNQaOz/a2bjf6cvzFwWZUHV9LMYo5mJiJk556Z5Dqd9AwDMz5kEEQnLuLQT"
                      "pN0Dms3AzBPkUWy1nZInve1eLI4ykTB7l6M+fvoEpsujYoi573sics6VZTlp+I0BvPSB0WxeADBKljUTkaqOrzx4hK+r4mpy"
                      "YWLuUnz66FFduaJaDTHnnMflzzlXVeWce2HOLwUAO1OGjelzeiOxwQwHAGAGY+G9ogDsgytg9HILtqwgYmF4McJls13MZTab"
                      "dWfnqppzTvsxm81CuDG73Qhg0rvt08+4/EQ6reKkATMTCYd6G28CSHZ1OelhnEpYiEzNjLntOiIiNe+9iIwYVLXrOuec9/6N"
                      "TehQ3bv3CQlEkPdFC/HoxgQzG21/wnAFIOn4XVWnOQEIM4Ahpz5FkFMYgLZtq6qalmAYhlEJfd+XZflmACYXnQAwE9tUy1xh"
                      "218qdnaFvSHR4arjeU9IOUWLwzAkn+uyqOu6rmtmLopifB2Aoiiapnk1gBudIzA7WGy3jhCELQ6kWYfBtmeVRKYmVEO9UIem"
                      "RreggXzohgi1yjuLQ+yTmusia+cwhG6rmyY3KhuECyq35ZKNIc4VZWr75vHT+/PlclaWi0pExjiRcxaR+XxeluUwDH3fT1Y9"
                      "KfNVGui6joRDKBuAiIqyLBZVlqpffOM8JQS2srroIwUs6tn52XqF6L3Ppk03kPgiBJbCKHs6W6/X3jQQ5SaVRU1Ep49ON/NF"
                      "512M5L2vZ7P6+Liu5uA0CXAYvse0MIW+L2VC1aw+7/uYO3f7pC7qoVk/e3bWX/jjpayK0iJ8E2sDMXtzVuQEKWfLFPuu2Wq2"
                      "PrXri2bbdjLL7WYroGU98+B4uQ0sR/XyWdv12ncDMmdmV9c1HHTf4BxYJonIGFKnGH0YKm4EMPSxrudS+rOUn20uHPLRyUlY"
                      "HnebnwUJn37y6eaiPTm+nRl9TorcoviV994pnfTd0DTdk9Nnn33xeLNtf951F5vm4tnF7ZNb7737Lvp+XpXf/u3f0uYXII45"
                      "x5Tb3DE7EACectYk5QggpXRoOa8H0A69eO9E4xCJXFmUj548/tsPf/jh5w+9FI9/eXZ+uh765Dzfvn2rKP07J0EYb9+7m1Jq"
                      "mqZtWwB1XQ+PnjUXm82mOX22+d8/+nGO8cFbJ5+vn3znvfu5WI3JO/WJnQAg0PWky8zOuWEYxjrqy5rQ0dHJF9tWhmZ28o7v"
                      "3Ecf/+ivfvHRz39y9u6vvf/TTz9Tsffe/6bFtCzDyruf/Oj/uju/gpyIjBh93xryydEqlNW333//xx9/3CRNzJfdZjEv+nb9"
                      "g7/8n5/xB1V5XHHhQFFN2AFI9pJ24DDbvACAiG7OA0QkXNaz3vsvfvbo448/XtxZ/e7vfvfZ089/+9f+Ls/nrq43Z2fnP//0"
                      "KOV/cPeD9Xx2tKgLod4s9S1irOrKifyvv/7w3oN3H5zc6bNeXpx32/OZ6j/9R//47HRdqGbLYpZzNiYAwzCU7qp6PfSECYCq"
                      "Hvr3jQCapgkhAP35+bl37v3339+WOD07C2/9RluUf/L97xVHq9//vX/x4Funn/z5X9yuqmWxVdWh6y1HaBImH8Sg3/wnv8MS"
                      "vvff/4dG/eff/e7pZ5/88M+/98133zk6kvl87gcjilPh+YJ5XDeY8c7h/RvzgNdMOTkmALPZ4u6t+7nRzz/67PhXf/Xf/cf/"
                      "/G//zR//hz/8o+//yX+rzddV+OL8ixnaUnvKSghelrBCEE4Wx7cW9/7Tn/7Z9//yh3/wx3/0Xz/8wdu/9eutWOR8/+17rLkE"
                      "a9eH0vcaAdTeT9X/YcQcK9Ou615oQl7lAyxhz1gxmM3MBTdbzj/8sz/91nH5r37vd3I23z/72x/+RV5/8Y23biVrpldOCZiI"
                      "fvE3P3Ddk7tB8/Hi6d/8n7+ibu7haHBuJiJiIiJ88zoeWvRLNXNzMaeSknmj4MuSSo/ybnGyDLdN2jt85KvZEG1WuFu1GxA8"
                      "9wYZk+ghAGa+M0vf+fW/85vv3R+G2DeNrJ988Pe+GRyEKIbgwM45MbmpVrsJwzReUY1SzFYoMbPz7OGX9fykWtZYr+c1l3XT"
                      "xZzjqg4obrMNqR1ebHEAM7t7a3l7OdfeSlecna/72C3vHl1uLygO0QdvlL2X/Hoq4o01wE6ISGHZdEhRdYisjikmK4oq1HMJ"
                      "eRgGEqQcu3Y4jBjMPDY3McYhmpeiLMmxf/DWOz1yk7piztSsO/FeKTnHeL0GrhcRr9VABpQINrYrIs45VkfFIqd8vmmNqSwr"
                      "QR46aWMuGVOcds6NNX3f95VKXVXC3PdJSi8mOfazepVS68hJgnOO7Etp4LCCmPRwo/ckDORMHEFATkJZuVCDwzalUM98WTCz"
                      "Dv3F2Vnq2lur5RihxyA9Asg5933fN23OOWpGgU3cXG7PPbJtN6PDTOoCXsYM32BCz1nKTQ+IwHshRzvJsg193G4bhammtt1S"
                      "HlazqnIWm4vt+Rn2he5Yfo0lcYwxbjZDsx1iW89L540R7x7NZWhHua+7zWvHCz3GKzTgDM6ScSYP1pySDUXlg9RDm2pfBQlP"
                      "Hz9dn7dA6f2CigpeyoqrkJEbTR0RK4d112RYHap+3XDkoqger9dD4RG1KgrkHui9I9VhLOYOjeQ5eVLy3o9M0WHWu7kjU9vv"
                      "bOzvGAwWY88Aw0YawnuPnLLi0aN1XZVkybINSfqBkUy8i2i6rivLEjARmWw2xjgWmC/QLa8eLxRCeG1TT7bfhRkNCVqGYJaH"
                      "rm8vL7ebi267bbbboW1+8NefPHjw4P7de86F9fn28y8en1904vxvfPu9YRhijCLMzJrzyDjYHgBujjDXzeb6z15PRDJAZlBV"
                      "UjPtUyvElmNKSUSWy1VZFJtN+OAffrBYLFLKm8vGleH49q3lCY6OTjz3AiI1kFrOOcfY9SklHCz/q1d6quFE5A0A0EgFqSnt"
                      "aAVkVaimnlhSSiPvWdc127KezROpE39+cdn0VAaRUKvqbCZ9RM55GAYWcs7F2Pd9r6r+eUPf0zeGayKO0r+0n8SrqcUdelXV"
                      "zKRGBkLhPIAxSWmOI4s2tF0oyAos57P5fG4kXdddnq/7bp3BXdcxszj23uccc854WVx/znSvaWAX9a9RdDcCEIwU3C66KykJ"
                      "GMg5Qo1MneNMgSBE8EU1CzwkTZqHjJR6EVnOlgO7p5utphz7ISeCmlm2rCIyhf0r6Qn7fY7X+MCXKiV2dJDtF2BPMDIo287z"
                      "VHPb7zo9bSwmuLI2uGbbO86ecLneusKNOUF1tAS94vwmuo5eFYWu92JfCkDmDXHdppTcNgG5Y+KQ4pmSeR9iG01D4Wafff7Z"
                      "41+eHq9OhofPnHOzWffJJ5+uVquu7ZfL5fn5+Wm5Prl168Hb949PTphEmYhF2ZAdt8NQ2JFU7fwor60/QYGeqBgFGAMUM+ec"
                      "t9vtofF8KVYCYBu7ByecGGQKMyODlsHnMm8uz04fP/vJRz99/MWT1erowT/7jp/PV2+99faTbznnPvnpp7+8uPg0bX+zns3m"
                      "VSgKcqSkqqrIyNDxgmkMr8Mw+GxwO7kngxkDQIyxruvrWnplT7zbwnDM46Y6zCzD2HJKg2msy2Lx3tur2ezJk6eklD5+lIDz"
                      "n37RnF/OZrPl+cXtEO7P7y7uhNliPpuXPjhFVlJVA6CGDCNhHwI5udhczpexdmDm0SZHMmsYhpGP8AfN2pdyYhJm5sQEQBVT"
                      "whEmACJUz4rVfHnn9tGdW6vLy8v+PA/DUIj4uq6rqlSrqsqOVukIvvRFVbBjVYMpmIjIWEyEhcQHA52dnttqu/KBi2Iqk2KM"
                      "bduORYT3/tCKJje4OYySI3ZEkhQxZ+SsRjBu+kFCUVRl7IdttyElDlTOC1341HQRSL1tBVu2TiKA27M5OwFTUqRs2WAkICLn"
                      "xDvyAmElnF+sy7Ztixw4jBWhmTVN03XdmG0Ol3/aVcFrM7FNdYRpzjmTRcNm24qIEJfBFT6EKkjHRfYB2jRd0iSgo7qq6zqE"
                      "otXBjIak2XLKOYOIiZlBzKEgZwaGcd8PZhRjas7Px2J2bCcAhBBGavqFFLHrPW4SfciajZiZRZgla4pqbFrNls12m/qm8IGd"
                      "CJlabIeeSvHLajEv6jxXHTNd3vYXRagNyBnZLCkbMRsrZLDsmEgkw9iJZhRFQZzSkEYRx1KlKIqRdn8pgFeG0ZxVVcDCnnee"
                      "oCmn2Fs0FEXlvfQpWpuruji6dUKXg3NBVZU1ZxWRnmKnfEFiBwmKwGBh9qpx3P1XM2YWkVAWTlCQmzYnnXOz2WwyntG0XnCD"
                      "GwGsMn1U6TLnW1szcU3JHoPJkLqSyedofTYnyMxDQuHYipBFCN4MbDyoNVE3CkgeYuyHFI0hwYVAEpTkls4v+w2vipjYG/39"
                      "9+/l9EyHmgoaRa+qaqI5JjyHbjACuxGAsoB8RlLKTCFLHLOx7M4tWc6qamOcbkUqdkRJiTUDLJotZVXQEHNKmhRmIAGgZErE"
                      "iYEQFOKkylwuju7l1d2KQ0zD1NNdjznXxyt3KQkqRJmVSMmBSXOWcZdVzTSZWdpHgwYEIBuZAUwwjjH3cWhjymYjC+3FASpk"
                      "TLmzRMx9H5MVXY4LX6oLA1Bc25Ccauk3A2Cpg0WhDAaEnGfTTAnJkplB1UDjbqqqQe2yb0bdGhjgbJRSylm7rGASERbHTkTE"
                      "exER0iSFo5xYNfatKDqLQv6Qu53GV9on1iwMISbNEAsMMksaOwIABdN43MRUNatqNsWoZyFTJM0xZVVl50aCn70j58l5iCPn"
                      "SsvkGcTefJBAQSpSQcr2XL352mbtZg2UC1Qrs1bREwcSE2Rjdi7knE2TmjIwlpc553HHe2xAdy0IFIA4YhJjAjirxqQmqmw+"
                      "8xDN+6rjmt0KEAYhm8ibnVa5uZQ4ec9Wj2O87PWCwepdwR5BPQshmfZECqhjGJIBEqFmRsamGQbNQuDCZ0DYWAgi7AOHwheV"
                      "K0qxYgjJnawSH7vqXvYLwEnSr+3AEy3v2eJ+3/i2zZwJ7Jg8XHbIyhBRo0TEZGD2IpQYKaqZgckD7MVIQghdjizehSChdkVZ"
                      "VPNyviiKalbc1zLO7h0XWJSz+9E7Gs8xPN8lv55yvPnQHy4vLx8+fPjw4cOzs7Occ1mWZVkuNGIfhicCy8xybCeabX/ShQF0"
                      "9dx7X4TKucDMAJsSgKz9yMrM5/PlcjkeK7p+VuS141UAVPXy8vL09PTx48dPnjxZr9dd1711+2j0MBEJIRRFMRKJnt2h6COD"
                      "QESDgCA70c12G43GxLksy8VisVgsprMcXyeAaS5V3W63p6enT58+vbi4+OSzn+WcR6/13hdFMWJYVMcTgFEzozaE094MGACP"
                      "oVRkuZzPZrPlcnmYX7/CeBUAXItiKSVladt2s9lsNpu2bWPcEROlLw9OVVzxnkW4YnBHintsTcqynM6gjLTBS2mf/y8TwsEB"
                      "sCkRWt7VebuEL/vaMA27GQ9OewBQ4ldbxXT24Yqm/hoBTHzGRI8F97KzRwpIfOkMan6Sf/T2fYYSXCtyvoIP3Gh/Yzl+PRGm"
                      "NOBgG2Z3l188xj2tiyl0f9gVV0WBXt+1/mrjRg0czj5yaaPECfqSH8P4hgwk++NRe2XqfomvTiTuRPlKYP4f2ZNncEHUFw4A"
                      "AAA4dEVYdGNvbW1lbnQAUHJvY2Vzc2VkIEJ5IGVCYXkgd2l0aCBJbWFnZU1hZ2ljaywgejEuMS4wLiB8fEIxxMuMpwAAACV0"
                      "RVh0ZGF0ZTpjcmVhdGUAMjAxOS0wNy0xNVQxNzozMDozNyswMDowMFFlOFMAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMTktMDct"
                      "MTVUMTc6MzA6MjQrMDA6MDDdeprsAAAAAElFTkSuQmCC\"}";
        auto len = strlen(json);
        *response = new PacketStatusResponse(String(len, json));
    } else if (*packet->packet_id == 0x01) {
        auto ping = dynamic_cast<PacketPing *>(packet);
        *response = new PacketPong(ping->payload);
        keep_alive_ = false;
    }
    return this;
}


mc::BasePacket *mc::ConnectionLogin::match_packet(mc::Varint::Int packet_id, mc::Buffer &packet) {
    UNUSED(packet_id);
    UNUSED(packet);
    throw mc::Exception(mc::ErrorType::kNotImplemented, "login");
}

mc::BaseConnection *mc::ConnectionLogin::handle_packet(mc::BasePacket *packet, BasePacket **response) {
    UNUSED(packet);
    UNUSED(response);
    throw mc::Exception(mc::ErrorType::kNotImplemented, "login");
}
