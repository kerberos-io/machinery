#include "gmock/gmock.h"
#include "capture/Image.h"

class MockImage : public kerberos::Image
{
    public:
        MOCK_METHOD0(getColumns, int());
        MOCK_METHOD0(getRows, int());
        MOCK_METHOD2(get, int(int y, int x));
};