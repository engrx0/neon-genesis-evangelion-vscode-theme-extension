import { readFile } from "node:fs/promises";

const manifest = JSON.parse(await readFile("package.json", "utf8"));
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

  const source = JSON.parse(await readFile(theme.path, "utf8"));
  if (!source.colors || !Array.isArray(source.tokenColors)) {
    throw new Error(`${theme.path} must define colors and tokenColors`);
  }
}

console.log(`Validated ${themes.length} theme contributions.`);
