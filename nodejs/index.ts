import {execFile as execCallback} from 'child_process';
import path from 'path';
import util from 'util';
import os from 'os';

const exec = util.promisify(execCallback);

const AEC3_PATHS: { [platform: string]: { [arch: string]: string } } = {
    darwin: {
        arm64: path.join(__dirname, '..', 'bin', 'aec3-darwin-arm64'),
    },
    linux: {
        x64: path.join(__dirname, '..', 'bin', 'aec3-linux-x64'),
    },
};

export const aec3 = async (input: { nearFile: string; farFile: string; outFile: string; }) => {
    const platform = process.env.npm_config_platform || os.platform();
    const arch = process.env.npm_config_arch || os.arch();
    const aec3Path = AEC3_PATHS[platform]?.[arch];
    if (!aec3Path) {
        throw new Error(`No AEC3 binary found for platform ${platform} and arch ${arch}`);
    }
    return exec(aec3Path, [input.farFile, input.nearFile, input.outFile]);
}