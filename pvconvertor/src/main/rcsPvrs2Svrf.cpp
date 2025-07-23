#include "rcsPvrs2Svrf.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <algorithm>
#include <stdexcept>

// Include necessary headers for the parser-based approach
#include "public/synnode/rcsSynRootNode.h"
#include "public/manager/rcErrorManager.h"
#include "public/manager/rcsManager.h"
#include "preprocess/rcsPreprocessor.h"
#include "lexical/rcsLexParser.h"
#include "syntax/rcsSynConvertor.h"
#include "public/synnode/rcsSynLayerDefinitionNode.h"
#include "public/synnode/rcsSynLayerOperationNode.h"
#include "public/synnode/rcsSynRuleCheckNode.h"

// Legacy struct, to be removed once parser-based implementation is complete
struct SyntaxMapping {
    std::string pvsPattern;
    std::string svrfTemplate;
    bool isRegex;
};

/**
 * @brief Main method to convert a PVS file to SVRF format
 *
 * This method orchestrates the conversion process:
 * 1. Tokenizes the PVS file
 * 2. Parses the tokens into an AST
 * 3. Generates SVRF output from the AST
 */
bool rcsPvrs2Svrf::execute(const std::string &pvsFile, const std::string &svrfFile, std::ostream &out)
{
    out << "Converting " << pvsFile << " to " << svrfFile << " (Parser-based approach)" << std::endl;
    
    try {
        // Step 1: Tokenize PVS file
        std::list<rcsToken_T> tokenStream;
        if (!tokenizePvsFile(pvsFile, tokenStream, out)) {
            out << "Error: Failed to tokenize PVS file" << std::endl;
            return false;
        }
        
        // Step 2: Parse tokens into AST
        rcsSynRootNode_T *pvsRoot = parsePvsTokens(tokenStream);
        if (!pvsRoot) {
            out << "Error: Failed to parse PVS tokens" << std::endl;
            return false;
        }
        
        // Step 3: Generate SVRF output from AST
        bool result = generateSvrfOutput(pvsRoot, svrfFile, out);
        
        // Clean up
        delete pvsRoot;
        
        if (result) {
            out << "Conversion complete." << std::endl;
        }
        
        return result;
    }
    catch (const std::exception &e) {
        out << "Error during conversion: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Tokenizes the PVS file into a list of tokens
 * 
 * This method uses the existing lexical analyzer to tokenize the PVS file.
 */
bool rcsPvrs2Svrf::tokenizePvsFile(const std::string &pvsFile, std::list<rcsToken_T> &tokens, std::ostream &out)
{
    try {
        // Create a set for storing comment line numbers (not really used but needed for interface)
        std::set<hvUInt32> commentLines;
        
        // Use the existing lexical analyzer to tokenize the PVS file
        rcsLexParser_T lexParser(tokens, commentLines);
        lexParser.parse(pvsFile.c_str());
        
        return true;
    }
    catch (const std::exception &e) {
        out << "Tokenization error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Parses the token stream into an Abstract Syntax Tree
 * 
 * This method uses the existing syntax parser to build an AST from the tokens.
 */
rcsSynRootNode_T* rcsPvrs2Svrf::parsePvsTokens(std::list<rcsToken_T> &tokens)
{
    try {
        // Use the existing syntax parser with PVS mode
        rcsSynConvertor parser(tokens, false);
        
        // Parse tokens into an AST
        std::map<hvUInt32, std::pair<hvUInt32, bool>> blankLines;
        rcsSynRootNode_T *root = parser.execute(blankLines);
        
        return root;
    }
    catch (const std::exception &e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return nullptr;
    }
}

/**
 * @brief Generates SVRF output from the AST
 * 
 * This method traverses the AST and generates equivalent SVRF code.
 */
bool rcsPvrs2Svrf::generateSvrfOutput(rcsSynRootNode_T *root, const std::string &svrfFile, std::ostream &out)
{
    try {
        // Open the output file
        std::ofstream outFile(svrfFile.c_str());
        if (!outFile.is_open()) {
            out << "Error: Cannot create SVRF file: " << svrfFile << std::endl;
            return false;
        }
        
        // Write header
        outFile << "// SVRF file generated from PVS\n\n";
        
        // Process each node in the AST
        for (auto *node : root->getChildNodes()) {
            convertNode(node, outFile);
        }
        
        outFile.close();
        return true;
    }
    catch (const std::exception &e) {
        out << "Error generating SVRF output: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Dispatches node conversion based on node type
 */
void rcsPvrs2Svrf::convertNode(rcsSynNode_T *node, std::ostream &out)
{
    // Dispatch based on node type
    switch (node->getNodeType()) {
        case rcsSynNode_T::LAYER_DEFINITION_NODE:
            convertLayerNode(static_cast<rcsSynLayerDefinitionNode_T*>(node), out);
            break;
        case rcsSynNode_T::LAYER_OPERATION_NODE:
            convertOperationNode(static_cast<rcsSynLayerOperationNode_T*>(node), out);
            break;
        case rcsSynNode_T::RULE_CHECK_NODE:
            convertRuleNode(static_cast<rcsSynRuleCheckNode_T*>(node), out);
            break;
        default:
            // Handle unknown node or write a comment
            out << "// Unhandled node type: " << node->getNodeType() << "\n";
            break;
    }
}

/**
 * @brief Converts a layer definition node to SVRF format
 */
void rcsPvrs2Svrf::convertLayerNode(rcsSynLayerDefinitionNode_T *node, std::ostream &out)
{
    // Extract layer name
    std::string layerName = node->getLayerName();
    
    // Format as SVRF layer definition
    out << "LAYER " << layerName << " ;\n";
}

/**
 * @brief Converts an operation node to SVRF format
 */
void rcsPvrs2Svrf::convertOperationNode(rcsSynLayerOperationNode_T *node, std::ostream &out)
{
    // Extract operation details
    std::string opName = node->getOperationName();
    std::string opType = node->getOperationType(); // AND, OR, etc.
    std::vector<std::string> operands = node->getOperandLayers();
    
    // Format as SVRF operation
    out << opName << " = " << opType << " ( ";
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i > 0) out << " ";
        out << operands[i];
    }
    out << " ) ;\n";
}

/**
 * @brief Converts a rule check node to SVRF format
 */
void rcsPvrs2Svrf::convertRuleNode(rcsSynRuleCheckNode_T *node, std::ostream &out)
{
    // Extract rule details
    std::string ruleName = node->getRuleName();
    
    // Start rule block
    out << "CHECK " << ruleName << "\n";
    
    // Process rule body
    for (auto *stmt : node->getStatements()) {
        convertRuleStatement(stmt, out);
    }
    
    // End rule block
    out << "END_CHECK\n\n";
}

/**
 * @brief Converts a rule statement to SVRF format
 */
void rcsPvrs2Svrf::convertRuleStatement(rcsSynStatement_T *stmt, std::ostream &out)
{
    // Handle different statement types (LAYOUT, SPACING, etc.)
    switch (stmt->getStatementType()) {
        case rcsSynStatement_T::LAYOUT_STMT:
            // Convert LAYOUT statement to SVRF
            out << "  INTERNAL1 = " << stmt->getLayerName() << " ;\n";
            break;
        case rcsSynStatement_T::SPACING_STMT:
            // Convert SPACING statement to SVRF
            out << "  SPACE ( INTERNAL1 ) " << stmt->getOperator() 
                << " " << stmt->getValue() << " ;\n";
            break;
        default:
            // Handle unknown statement or write a comment
            out << "  // Unhandled statement type\n";
            break;
    }
}

// Legacy implementation - will be removed once parser-based implementation is complete
std::string rcsPvrs2Svrf::processPVSContent(const std::string &content)
{
    std::string result = content;
    
    // Convert header
    result = convertHeader(result);
    
    // Convert layer definitions
    result = convertLayers(result);
    
    // Convert operations
    result = convertOperations(result);
    
    // Convert rules
    result = convertRules(result);
    
    return result;
}

// Additional methods for specific conversions
std::string rcsPvrs2Svrf::convertHeader(const std::string &content) {
    // Convert PVS header elements to SVRF
    std::string result = content;
    
    // Handle VERILOG section - match any VERILOG line regardless of whitespace
    std::regex verilog_regex(R"(VERILOG\s*)");
    result = std::regex_replace(result, verilog_regex, "// VERILOG converted to SVRF format\n");
    
    // Handle special comments that might be in the file
    std::regex begin_switch_regex(R"(/\*\s*==\s*begin\s+switch\s+unknow\s+segment\s*==\s*\*/)");
    result = std::regex_replace(result, begin_switch_regex, "// Begin SVRF section");
    
    std::regex end_switch_regex(R"(/\*\s*==\s*end\s+switch\s+unknow\s+segment\s*==\s*\*/)");
    result = std::regex_replace(result, end_switch_regex, "// End SVRF section");
    
    return result;
}

std::string rcsPvrs2Svrf::convertLayers(const std::string &content) {
    // Convert PVS LAYER statements to SVRF LAYER statements
    std::string result;
    
    // Match both standard layer format and parenthesized format
    // Pattern 1: LAYER Metal1;
    std::regex layer_regex1(R"(LAYER\s+(\w+)\s*;)");
    // Pattern 2: layer ( Metal1 ; )
    std::regex layer_regex2(R"(layer\s*\(\s*(\w+)\s*;\s*\))");
    
    // Process standard format
    std::sregex_iterator iter1(content.begin(), content.end(), layer_regex1);
    std::sregex_iterator end;
    size_t last_pos = 0;
    
    while (iter1 != end) {
        // Copy content before this match
        result += content.substr(last_pos, iter1->position() - last_pos);
        // Convert to SVRF LAYER statement
        result += "LAYER " + (*iter1)[1].str() + " ;\n";
        last_pos = iter1->position() + iter1->length();
        ++iter1;
    }
    
    // Process result with parenthesized format
    std::string temp_result = result;
    result.clear();
    std::sregex_iterator iter2(temp_result.begin(), temp_result.end(), layer_regex2);
    last_pos = 0;
    
    while (iter2 != end) {
        // Copy content before this match
        result += temp_result.substr(last_pos, iter2->position() - last_pos);
        // Convert to SVRF LAYER statement
        result += "LAYER " + (*iter2)[1].str() + " ;\n";
        last_pos = iter2->position() + iter2->length();
        ++iter2;
    }
    
    // Copy the rest from temp_result if we processed it, or from content if not
    if (temp_result.empty()) {
        result += content.substr(last_pos);
    } else {
        result += temp_result.substr(last_pos);
    }
    
    return result;
}

std::string rcsPvrs2Svrf::extractSection(const std::string &content, const std::string &startMarker, const std::string &endMarker) {
    size_t start = content.find(startMarker);
    if (start == std::string::npos) {
        return "";
    }
    start += startMarker.length();
    
    size_t end = content.find(endMarker, start);
    if (end == std::string::npos) {
        return content.substr(start);
    }
    
    return content.substr(start, end - start);
}

std::string rcsPvrs2Svrf::transformRule(const std::string &ruleContent) {
    std::string result = ruleContent;
    
    // Convert LAYOUT statements
    std::regex layout_regex(R"(LAYOUT\s+(\w+);)");
    result = std::regex_replace(result, layout_regex, "INTERNAL1 = $1 ;");
    
    // Convert SPACING statements
    std::regex spacing_regex(R"(SPACING\s+(<|>|<=|>=|==)\s+([\d\.]+);)");
    result = std::regex_replace(result, spacing_regex, "SPACE ( INTERNAL1 ) $1 $2 ;");
    
    return result;
}

std::string rcsPvrs2Svrf::convertOperations(const std::string &content) {
    // Convert PVS OPERATION statements to SVRF format
    std::string result;
    
    // Match standard operation format: OPERATION AND op1 Metal1 Metal2;
    std::regex operation_regex1(R"(OPERATION\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s*;)");
    
    // Match geom_and format: geom_and ( OPERATION op1 Metal1 "Metal2;" )
    std::regex operation_regex2(R"(geom_and\s*\(\s*OPERATION\s+(\w+)\s+(\w+)\s+"?(\w+)"?\s*;?\s*\))");
    
    // Process standard format
    std::sregex_iterator iter1(content.begin(), content.end(), operation_regex1);
    std::sregex_iterator end;
    size_t last_pos = 0;
    
    while (iter1 != end) {
        // Copy content before this match
        result += content.substr(last_pos, iter1->position() - last_pos);
        
        // Extract the operation components
        std::string op_type = (*iter1)[1].str();
        std::string op_name = (*iter1)[2].str();
        std::string op1 = (*iter1)[3].str();
        std::string op2 = (*iter1)[4].str();
        
        // Format as SVRF operation
        result += op_name + " = " + op_type + " ( " + op1 + " " + op2 + " ) ;";
        
        last_pos = iter1->position() + iter1->length();
        ++iter1;
    }
    
    // Process geom_and format
    std::string temp_result = result.empty() ? content : result;
    result.clear();
    std::sregex_iterator iter2(temp_result.begin(), temp_result.end(), operation_regex2);
    last_pos = 0;
    
    while (iter2 != end) {
        // Copy content before this match
        result += temp_result.substr(last_pos, iter2->position() - last_pos);
        
        // Extract the operation components
        std::string op_name = (*iter2)[1].str();
        std::string op1 = (*iter2)[2].str();
        std::string op2 = (*iter2)[3].str();
        
        // Clean up the op2 string (remove quotes and semicolons)
        op2.erase(std::remove(op2.begin(), op2.end(), '"'), op2.end());
        op2.erase(std::remove(op2.begin(), op2.end(), ';'), op2.end());
        
        // Format as SVRF operation
        result += op_name + " = AND ( " + op1 + " " + op2 + " ) ;";
        
        last_pos = iter2->position() + iter2->length();
        ++iter2;
    }
    
    // Copy the rest
    result += temp_result.substr(last_pos);
    return result;
}

std::string rcsPvrs2Svrf::convertRules(const std::string &content) {
    std::string result = content;
    
    // First match: Handle RULE...{ } blocks
    std::regex rule_block_regex(R"(RULE\s+(\w+)\s*\{([\s\S]*?)\})");
    result = std::regex_replace(result, rule_block_regex, "CHECK $1\n$2\nEND_CHECK");
    
    // Handle standalone CHECK...END_CHECK format (already in correct format)
    // Make sure we don't process it multiple times
    std::regex existing_check_regex(R"(CHECK\s+(\w+)\s+([\s\S]*?)END_CHECK)");
    
    // Fix the format of partially converted checks
    std::regex partial_check_regex(R"(CHECK\s+(\w+)([^E]*))");
    result = std::regex_replace(result, partial_check_regex, "CHECK $1\n$2\nEND_CHECK");
    
    // Convert LAYOUT statements within rules to SVRF internal variables
    std::regex layout_regex(R"(LAYOUT\s+(\w+)\s*;)");
    result = std::regex_replace(result, layout_regex, "INTERNAL1 = $1 ;");
    
    // Convert SPACING statements to SVRF space checks
    std::regex spacing_regex(R"(SPACING\s+(<|>|<=|>=|==)\s+([\d\.]+)\s*;)");
    result = std::regex_replace(result, spacing_regex, "SPACE ( INTERNAL1 ) $1 $2 ;");
    
    return result;
}
