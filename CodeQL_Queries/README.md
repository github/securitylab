To run the queries in this repository:
1. Click the green "Code" button > Codespaces > Create a codespace on main. A new codespace will be created for you with VS Code CodeQL extension preinstalled.
2. When the codespace finishes setting up, open the terminal, and find the path to the codeql binary (which comes preinstalled with the VS Code CodeQL extension) with the command:
```bash
find ~ -type f -name codeql -executable 2>/dev/null
```
It will most likely look similar to this:
```
/home/codespace/.vscode-remote/data/User/globalStorage/github.vscode-codeql/distribution1/codeql/codeql
```
3. Go to the language folder with the query you want to run, and using the CodeQL binary, run `codeql pack install`. For example:
```bash
cd cpp
/home/codespace/.vscode-remote/data/User/globalStorage/github.vscode-codeql/distribution1/codeql/codeql pack install
```
This will install the CodeQL library files required to run the CodeQL queries.
4. Press `Ctrl/Cmd + Shift + R` to reload the window to see syntax highlighting etc. 
5. Check the README in the folder with the query you are interested in, and add the database listed in the README to your VS Code CodeQL extension.
6. Open the query file you are interested in, right-click and choose `CodeQL: Run Query on selected database` from the dropdowns.
