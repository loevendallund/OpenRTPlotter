#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <memory>

namespace OpenRTP
{
    class Util
    {
    public:
        Util();
        ~Util();

        glm::vec4 GetUniqueColor();
    private:
        class Impl;
        std::unique_ptr<Impl> PImpl;
    };
}
