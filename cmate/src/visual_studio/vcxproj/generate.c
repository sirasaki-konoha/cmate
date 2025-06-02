#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "visual_studio/vcxproj/generate.h"
#include "visual_studio/vcxproj/defines/project_configuration.h"

/* Function to generate a Visual Studio vcxproj file (.vcxproj) */
char *generate_vcxproj_xml(const char *project_name, const char *project_guid, const char *source_files[], const char *include_files[]){

    char *property_groups = format_string(
        "  <PropertyGroup Label=\"Globals\">\n"
        "    <VCProjectVersion>17.0</VCProjectVersion>\n"
        "    <Keyword>Win32Proj</Keyword>\n"
        "    <ProjectGuid>{%s}</ProjectGuid>\n"
        "    <RootNamespace>%s</RootNamespace>\n"
        "    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>\n"
        "  </PropertyGroup>\n",
        project_guid, project_name
    );

    char **source_files_xml = malloc(sizeof(char*) * 2);
    source_files_xml[0] = safe_strdup("  <ItemGroup>\n");
    source_files_xml[1] = NULL;

    for (int i = 0; source_files[i] != NULL; i++) {
        char *file_xml = format_string(
            "    <ClCompile Include=\"%s\" />\n", include_files[i]
        );
        source_files_xml = realloc(source_files_xml, sizeof(char*) * (i + 2));
        source_files_xml[i] = file_xml;
        source_files_xml[i + 1] = NULL;
    }


    for (int i = 0; include_files[i] != NULL; i++) {
        char *file_xml = format_string(
            "    <ClInclude Include=\"%s\" />\n", include_files[i]
        );
        source_files_xml = realloc(source_files_xml, sizeof(char*) * (i + 2));
        source_files_xml[i] = file_xml;
        source_files_xml[i + 1] = NULL;
    }

    char *source_files_group = format_string(
        "%s</ItemGroup>\n", 
        join_strings(source_files_xml, "")
    );

    char *project_configuration = PROJECT_CONFIGURATION_XML;
    char *vcxproj_xml = format_string(
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
        "%s"
        "%s"
        "%s"
        "</Project>\n",
        property_groups, project_configuration, source_files_group
    );

    printf("Generated vcxproj XML for project '%s'\n", project_name);

    free(property_groups);
    free_args(source_files_xml);
    return vcxproj_xml;

}



