#include "Project.h"

#include "Parser.h"

Project::Project()
{
}

void Project::readProject(QByteArray &buff)
{
    //    PROJ_buffer_s * p_proj = reinterpret_cast<PROJ_buffer_s*>(buff.data());
    oxitools::DataReader reader { (char *)buff.data(), static_cast<size_t>(buff.size()) };
    deserialize_PROJ_buffer_s(reader, project_data_);


}

void Project::readPattern(QByteArray &buff, uint16_t pattern_index)
{
    //    PROJ_buffer_s * p_proj = reinterpret_cast<PROJ_buffer_s*>(buff.data());
    oxitools::DataReader reader { (char *)buff.data(), static_cast<size_t>(buff.size()) };

    uint16_t seq_index = pattern_index / 16;
    pattern_index = pattern_index % 16;
    uint8_t type = buff[0];
    pattern_data_[seq_index][pattern_index].readPattern(buff.data(), type);

   //   project_data_.serialize(reader, project_data_.patterns[seq_index][pattern_index]);


}
