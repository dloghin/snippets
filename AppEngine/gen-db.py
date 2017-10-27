#!/usr/bin/python
#
# Generate Objectify Database entities from .yaml description
#
# Copyright (c) 2017 - Dumitrel Loghin
#

import yaml, sys, os, shutil, pprint

def main(argv):
    if (len(argv) < 3):
        print "Usage:", argv[0], "<db.yaml> <output_folder>"
        return
    
    # create output dir
    path = argv[2] + "/"
    try:
        shutil.rmtree(path)
    except:
        print "INFO: No output dir to delete", path
    os.mkdir(path)
    
    # read YAML    
    stream = file(argv[1], 'r')
    data = yaml.load(stream)
    
    # pprint.pprint(data)
    
    config_id_name = "id"
    config_id_type = "Long"
    config_id_generate = ""
    config_db_version = "1L"
    config_package = ""
    config_imports = []
    
    # Config
    if "__config" in data:
        configs = data["__config"]
        if "package" in configs:
            config_package = configs["package"]
        else:
            print "ERROR: please specify a package"
            return
        for key in configs:
            if key == "id":
                if "name" in configs[key] and "type" in configs[key]:
                    config_id_name = configs[key]["name"]
                    config_id_type = configs[key]["type"]
                    if not (config_id_type == "String" or config_id_type == "Long"):
                        print "ERROR: id must be String or Long only!"
                        return
                if "generate" in configs[key]:
                    config_id_generate = configs[key]["generate"]
            if key == "version":
                config_db_version = configs["version"]
            if key == "import":
                for item in configs["import"]:
                    if item not in config_imports:
                        config_imports.append(item)
    else:
        print "ERROR: please specify database configs"
        return               
    
    # Classes
    for key in data:
        if not key == "__config":
            f = open(path + key + ".java", "w")
            
            # Package
            f.write("package " + config_package + ";\n\n")
            
            # Imports
            imports = []
            for field in data[key]:
                if "import" in data[key][field]:
                    for item in data[key][field]["import"]:
                        if item not in imports:
                            imports.append(item)
                
            f.write("import java.io.Serializable;\n")
            f.write("import java.util.List;\n")
            f.write("\n")
            f.write("import com.googlecode.objectify.annotation.Entity;\n")
            f.write("import com.googlecode.objectify.annotation.Id;\n")
            f.write("import com.googlecode.objectify.annotation.Index;\n")
            f.write("\n")

            if len(imports):                
                for item in imports:
                    f.write("import " + item + ";\n")
                f.write("\n")

            if len(config_imports):                
                for item in config_imports:
                    f.write("import " + item + ";\n")
                f.write("\n")
            
            # Class
            f.write("@Entity\n")
            f.write("public class " + key + " implements Serializable {\n")
            
            # Fields
            f.write("\tprivate static final long serialVersionUID = " + config_db_version + ";\n\n")
            f.write("\t@Id\n\tprivate " + config_id_type + " " + config_id_name + ";\n\n")
            defaults = {}
            haveLists = False
            for field in data[key]:
                if "type" in data[key][field]:
                    type = data[key][field]["type"]
                else:
                    print "ERROR: no type for field", field
                    return
                if "default" in data[key][field]:
                    defaults[field] = data[key][field]["default"]
                if "list" in data[key][field] and data[key][field]["list"]:
                    haveLists = True
                if "comment" in data[key][field]:
                    f.write("\t/* " + data[key][field]["comment"] + " */\n")
                if "index" in data[key][field] and data[key][field]["index"]:
                    f.write("\t@Index\n")
                f.write("\tprivate " + type + " " + field + ";\n\n")
            
            # Custom methods
            f.write("\t/* Custom Methods */\n")
            if len(defaults) > 0 or haveLists or len(config_id_generate) > 0:
                f.write("\tpublic " + key + "() {\n")
            if len(config_id_generate) > 0:
                f.write("\t\t" + config_id_name + " = " + config_id_generate + ";\n")
            if len(defaults) > 0:
                for field in defaults:
                    f.write("\t\t" + field + " = " + defaults[field] + ";\n")
            if haveLists:
                for field in data[key]:
                    type = data[key][field]["type"]
                    if "list" in data[key][field] and data[key][field]["list"]:
                        f.write("\t\t" + field + " = new " + type + "();\n")
            if len(defaults) > 0 or haveLists or len(config_id_generate) > 0:                               
                f.write("\t}\n\n")
            
            # Database methods
            f.write("\t/* Database Methods */\n")
            f.write("\tpublic static void add" + key + "(" + key + " val) {\n")
            f.write("\t\tnew DataBaseManager<" + key + ">().addObject(val);\n}\n\n")
            
            f.write("\tpublic static void update" + key + "(" + key + " val) {\n")
            f.write("\t\tnew DataBaseManager<" + key + ">().updateObject(val);\n}\n\n")

            f.write("\tpublic static " + key + " getObjectById(" + config_id_type + " id) {\n")
            if config_id_type == "Long":
                f.write("\t\treturn new DataBaseManager<" + key + ">().getObjectByLongId(" + key + ".class, id);\n\t}\n\n")
            else:
                f.write("\t\treturn new DataBaseManager<" + key + ">().getObjectByStringId(" + key + ".class, id);\n\t}\n\n")
            
            f.write("\tpublic static " + key + " getObjectByField(String fieldName, String fieldVal) {\n")
            f.write("\t\treturn new DataBaseManager<" + key + ">().getObjectByField(" + key + ".class, fieldName, fieldVal);\n\t}\n\n")
            
            f.write("\tpublic static List<" + key + "> getObjectsByField(String fieldName, String fieldVal) {\n")
            f.write("\t\treturn new DataBaseManager<" + key + ">().getObjectsByField(" + key + ".class, fieldName, fieldVal);\n\t}\n\n")
            
            # Setter and Getter
            f.write("\t/* Setter and Getter */\n")
            f.write("\tpublic " + config_id_type + " getId() {\n\t\treturn id;\n\t}\n\n")
            for field in data[key]:
                type = data[key][field]["type"]
                name = field[0].upper() + field[1:]
                f.write("\tpublic " + type + " get" + name + "() {\n\t\treturn " + field + ";\n\t}\n\n")
                f.write("\tpublic void set" + name + "(" + type + " val) {\n\t\tthis." + field + " = val;\n\t}\n\n")
            
            # Close
            f.write("}\n")
            f.close()
    
if __name__ == '__main__':
    main(sys.argv)
