"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
const express = require("express");
const openai_1 = require("openai");
const gptService_1 = require("../services/gptService");
const router = express.Router();
const openai = new openai_1.default({ apiKey: process.env.OPENAI_API_KEY });
router.post('/modify_scene', (req, res) => {
    const scene = req.body;
    //scene.width = 1980;
    console.log(scene);
    res.json(scene);
});
router.get('/test', (req, res) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const completion = yield openai.chat.completions.create({
            messages: [
                { "role": "system", "content": "you are a good scene modifier. Use the provided function to answer questions." },
                { "role": "user", "content": "Move the position of nano by 10 in the x direction, rotate 90 degrees from the y-axis, scaling twice as big in the z direction" },
            ],
            "model": "gpt-3.5-turbo-1106",
            "tools": gptService_1.tools,
        });
        console.log(JSON.stringify(completion.choices[0]));
        res.send(completion.choices[0]);
    }
    catch (err) {
        res.send(err);
    }
}));
exports.default = router;
//# sourceMappingURL=gpt.js.map