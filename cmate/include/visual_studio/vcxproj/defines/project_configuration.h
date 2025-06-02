#ifndef _PROJECT_CONFIGURATION_H_
#define _PROJECT_CONFIGURATION_H_

/* Including Relase x86_64 and Debug x86_64 build */
#define PROJECT_CONFIGURATION_XML \
    "  <ItemGroup Label=\"ProjectConfigurations\">\n" \
    "    <ProjectConfiguration Include=\"Debug|x64\">\n" \
    "      <Configuration>Debug</Configuration>\n" \
    "      <Platform>x64</Platform>\n" \
    "    </ProjectConfiguration>\n" \
    "    <ProjectConfiguration Include=\"Release|x64\">\n" \
    "      <Configuration>Release</Configuration>\n " \
    "      <Platform>x64</Platform>\n" \
    "    </ProjectConfiguration>\n" \
    "  </ItemGroup>\n"

#endif /* _PROJECT_CONFIGURATION_H */