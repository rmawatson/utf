import os
import os.path
import re
import datetime

prefix_text = """/* Copyright (c) 2018, rmawatson@hotmail.com
 *
 * Distributed under the Boost Software License, Version 1.0 (found in the
 * LICENSE file in the root directory of this source tree)
 */
 
/* this file has been automatically generated on %s and should not be edited manually. */
 
#pragma once
""" % datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
postfix_text = ""

include_regex = re.compile(r'#include\s+?([<"])([\w\.]+)[">].*?\r?\n')

standard_striptext = [
        re.compile(r"#pragma\s+once.*\r?\n"),
        include_regex
]

extract_includes = re.compile('\s*?#include\s+[<"]([\w.]+)[">]')


class include_node(object):
    def __init__(self,name):
        self.name = name
        self.includes = []
        self.included_by = []
        self.seen = False
        self.content = None
    
    def __eq__(self,other):
        return self.name == other.name
    
    def __repr__(self):
        return self.name
    
    __hash__ = None
    
def get_include_node(node_list,name):
    current_node = None
    for node in node_list:
        if node.name == name:
            current_node = node
            break
    
    if not current_node:
        current_node = include_node(name)
        node_list.append(current_node)
    return current_node
    
def make_single_header(includes,tracked_includes,strip_regexs,prefix_text,postfix_text,output):

    include_nodes = []
    extra_includes = []
    
    #gather includes
    for filename in [fn for fn in os.listdir(includes) if fn.endswith(".h")]:
        
        incfile = open(os.path.join(includes,filename),"r")
        content = incfile.read()
        
        include_node = get_include_node(include_nodes,filename)
        
        content_includes = include_regex.findall(content)
        
        for inc_type,content_include in content_includes:
            if content_include in tracked_includes:
                included_node = get_include_node(include_nodes,content_include)
                if not included_node in include_node.includes:
                    include_node.includes.append(included_node)
                if not included_node in include_node.included_by:
                    included_node.included_by.append(include_node)
            else:
                extra_includes.append((inc_type,content_include))
        
        for strip_re in strip_regexs:
            content,_ = strip_re.subn("",content)

        include_node.content = content
      
    sf_content_list = []
    #find the root node
    root = [node for node in include_nodes if len(node.included_by) == 0][0]
    
    def gather_content(parent_node):
        for node in (n for n in parent_node.includes if not n.seen):
            gather_content(node)
            node.seen = True
            print(node.name)
            sf_content_list.append(node.content)
     
    gather_content(root)
    
    sf_include_file = open(output,"w")
    
    sf_include_file.write(prefix_text)
    for inc_type,extra_include in extra_includes:
        if inc_type == "<":
            sf_include_file.write("#include <%s>\n" % extra_include)
        else: 
            sf_include_file.write('#include "%s\n"' % extra_include)
            
    for content in sf_content_list:
        sf_include_file.write(content)
    sf_include_file.close()
    
    
    
if __name__ == "__main__":

    includes = "../include/utf"
    tracked_includes = [fn for fn in os.listdir(includes) if fn.endswith(".h")]
    strip_regexs = standard_striptext + [
        re.compile(r"/\* Copyright.+?\*/",re.DOTALL)
    ]
    make_single_header(includes,tracked_includes,strip_regexs,prefix_text,postfix_text,"../single_include/utf/utf.h")
