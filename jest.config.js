
module.exports = {
    clearMocks: true,
    moduleFileExtensions: ['js'],
    testEnvironment: 'node',
    testMatch: ['**/*.test.js'],
    transform: {
      '^.+\\.ts$': 'ts-jest',
    },
    transformIgnorePatterns: ['^.+\\.js$'],
    verbose: true,
  }
