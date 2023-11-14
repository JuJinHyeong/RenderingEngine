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
const router = express.Router();
const openai = new openai_1.default({ apiKey: process.env.OPENAI_API_KEY });
router.get('/', (req, res) => __awaiter(void 0, void 0, void 0, function* () {
    const completion = yield openai.chat.completions.create({
        messages: [
            { "role": "system", "content": "you are very helpful assistant" },
            { "role": "user", "content": "give me capital of korea" }
        ],
        "model": "gpt-3.5-turbo-1106"
    });
    res.send(completion.choices[0]);
}));
exports.default = router;
//# sourceMappingURL=gpt.js.map