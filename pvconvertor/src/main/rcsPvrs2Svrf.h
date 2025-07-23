#ifndef RCS_PVRS2SVRF_H
#define RCS_PVRS2SVRF_H

#include <string>
#include <iostream>
#include <list>
#include <map>
#include <set>

#include "public/rcsTypes.h"
#include "public/token/rcsToken.h"

// Forward declarations
class rcsSynRootNode_T;
class rcsSynNodeConvertor_T;
class rcsSynLayerDefinitionNode_T;
class rcsSynLayerOperationNode_T;
class rcsSynRuleCheckNode_T;
class rcsSynNode_T;
class rcsSynStatement_T;

/**
 * @class rcsPvrs2Svrf
 * @brief Converts PVS format to SVRF format using a parser-based approach.
 *
 * This class utilizes the existing lexical and syntax analysis infrastructure
 * to parse PVS files and generate equivalent SVRF output.
 */
class rcsPvrs2Svrf {
public:
    rcsPvrs2Svrf() : m_lastGlobalLine(0) {}
    ~rcsPvrs2Svrf() {}
    
    void setLastGlobalLine(hvUInt32 n) { m_lastGlobalLine = n; }
    hvUInt32 getLastGlobalLine() const { return m_lastGlobalLine; }
    
    /**
     * @brief Main method to convert a PVS file to SVRF format
     * @param pvsFile Input PVS file path
     * @param svrfFile Output SVRF file path
     * @param out Stream for status messages
     * @return Success or failure
     */
    bool execute(const std::string &pvsFile, const std::string &svrfFile, std::ostream &out);
    
private:
    // Parser-based implementation methods
    
    /**
     * @brief Tokenize the PVS file content
     * @param pvsFile Path to the PVS file
     * @param tokens Output list of tokens
     * @return Success or failure
     */
    bool tokenizePvsFile(const std::string &pvsFile, std::list<rcsToken_T> &tokens, std::ostream &out);
    
    /**
     * @brief Parse tokens into an AST
     * @param tokens List of tokens
     * @return Root node of the AST
     */
    rcsSynRootNode_T* parsePvsTokens(std::list<rcsToken_T> &tokens);
    
    /**
     * @brief Generate SVRF output from AST
     * @param root Root node of the AST
     * @param svrfFile Output file path
     * @param out Stream for status messages
     * @return Success or failure
     */
    bool generateSvrfOutput(rcsSynRootNode_T *root, const std::string &svrfFile, std::ostream &out);
    
    // Node conversion methods
    void convertNode(rcsSynNode_T *node, std::ostream &out);
    void convertLayerNode(rcsSynLayerDefinitionNode_T *node, std::ostream &out);
    void convertOperationNode(rcsSynLayerOperationNode_T *node, std::ostream &out);
    void convertRuleNode(rcsSynRuleCheckNode_T *node, std::ostream &out);
    void convertRuleStatement(rcsSynStatement_T *stmt, std::ostream &out);
    
    // Legacy methods (to be removed once parser-based implementation is complete)
    std::string processPVSContent(const std::string &content);
    std::string convertHeader(const std::string &content);
    std::string convertLayers(const std::string &content);
    std::string convertOperations(const std::string &content);
    std::string convertRules(const std::string &content);
    std::string extractSection(const std::string &content, const std::string &startMarker, const std::string &endMarker);
    std::string transformRule(const std::string &ruleContent);
    
    hvUInt32 m_lastGlobalLine; // Last global line number (for included files)
};

#endif // RCS_PVRS2SVRF_H
