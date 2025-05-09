#include "stdint.h"
#include "string.h"
namespace eerl{

  typedef struct __attribute__ ((__packed__)) ttnmapbuf{
    float f_lat;
    float f_lon;
    float alt;
    uint32_t hdop;
    uint16_t i_bat;
  }ttnmapbuf_t;

  class TtnPackager{
    ttnmapbuf_t vals;

    public:
    void setLat(float lat){vals.f_lat = lat;}
    void setLon(float lat){vals.f_lon = lat;}
    void setAlt(float lat){vals.alt = lat;}
    void setHdop(int lat){vals.hdop = lat;}
    void packBuffer(char * buf, size_t nc){
      if (nc < sizeof(struct ttnmapbuf))
        return;

      memcpy(buf, &vals, sizeof(struct ttnmapbuf));
    };
    void packBuffer(ttnmapbuf_t * buf){
      memcpy(buf, &vals, sizeof(struct ttnmapbuf));
    }
  };
}