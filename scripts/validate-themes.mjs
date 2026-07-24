import { readFile } from "node:fs/promises";

function stripJsonComments(source) {
  let result = "";
  let inString = false;
  let escaped = false;
  let inLineComment = false;
  let inBlockComment = false;

  for (let index = 0; index < source.length; index += 1) {
    const character = source[index];
    const nextCharacter = source[index + 1];

    if (inLineComment) {
      if (character === "\n") {
        inLineComment = false;
        result += character;
      }
      continue;
    }

    if (inBlockComment) {
      if (character === "*" && nextCharacter === "/") {
        inBlockComment = false;
        index += 1;
      } else if (character === "\n") {
        result += character;
      }
      continue;
    }

    if (inString) {
      result += character;
      if (escaped) {
        escaped = false;
      } else if (character === "\\") {
        escaped = true;
      } else if (character === "\"") {
        inString = false;
      }
      continue;
    }

    if (character === "\"") {
      inString = true;
      result += character;
    } else if (character === "/" && nextCharacter === "/") {
      inLineComment = true;
      index += 1;
    } else if (character === "/" && nextCharacter === "*") {
      inBlockComment = true;
      index += 1;
    } else {
      result += character;
    }
  }

  return result;
}

function parseJsonc(source, path) {
  try {
    return JSON.parse(
      stripJsonComments(source).replace(/,\s*([}\]])/g, "$1"),
    );
  } catch (error) {
    throw new Error(`Unable to parse ${path}: ${error.message}`);
  }
}

const manifest = parseJsonc(await readFile("package.json", "utf8"), "package.json");
const themes = manifest?.contributes?.themes;

if (!Array.isArray(themes) || themes.length === 0) {
  throw new Error("package.json must contribute at least one theme");
}

const labels = new Set();
const paths = new Set();

for (const theme of themes) {
  if (!theme.label || !theme.path || theme.uiTheme !== "vs-dark") {
    throw new Error(`Invalid theme contribution: ${JSON.stringify(theme)}`);
  }
  if (labels.has(theme.label) || paths.has(theme.path)) {
    throw new Error(`Duplicate theme label or path: ${theme.label}`);
  }

  labels.add(theme.label);
  paths.add(theme.path);

  const source = parseJsonc(
    await readFile(theme.path, "utf8"),
    theme.path,
  );
  if (!source.colors || !Array.isArray(source.tokenColors)) {
    throw new Error(`${theme.path} must define colors and tokenColors`);
  }
}

console.log(`Validated ${themes.length} theme contributions.`);
