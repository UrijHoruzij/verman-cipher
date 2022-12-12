#include <node.h>
#include <fstream>
#include <string>

using v8::Boolean;
using v8::Context;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

namespace verman
{
    bool VernamCipher(char *in, char *out, char *key)
    {
        std::ifstream file;
        std::ofstream cipherfile;
        std::ifstream keyfile;
        file.open(in);
        cipherfile.open(out);
        keyfile.open(key);
        if ((!file.is_open()) || (!cipherfile.is_open()) || (!keyfile.is_open()))
            return false;
        char byte;
        char keybyte;
        while (file.read(&byte, sizeof(char)))
        {
            keyfile.read(&keybyte, sizeof(char));
            char resbyte = keybyte ^ byte;
            cipherfile.write(&resbyte, sizeof(char));
        }
        file.close();
        cipherfile.close();
        keyfile.close();
        return true;
    }
    long long getSizeFile(char *filename)
    {
        std::ifstream file;
        file.open(filename);
        file.seekg(0, file.end);
        long long res = file.tellg();
        file.seekg(0, file.beg);
        file.close();
        return res;
    }
    void generateKey(char *filename, long long size)
    {
        std::ofstream keyfile;
        keyfile.open(filename);
        while (size > 0)
        {
            char byte = rand() % 256;
            keyfile.write(&byte, sizeof(char));
            size--;
        }
        keyfile.close();
    }
    void Method(const FunctionCallbackInfo<Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();
        srand(time(0));

        if (args.Length() < 3)
        {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
            return;
        }
        if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString())
        {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
            return;
        }
        String::Utf8Value in(isolate, args[0]);
        String::Utf8Value out(isolate, args[1]);
        String::Utf8Value key(isolate, args[2]);

        std::string inStr(*in);
        std::string outStr(*out);
        std::string keyStr(*key);

        char *inChr = const_cast<char *>(inStr.c_str());
        char *outChr = const_cast<char *>(outStr.c_str());
        char *keyChr = const_cast<char *>(keyStr.c_str());

        long long sizeFile = getSizeFile(inChr);
        if (sizeFile == -1)
        {
            Local<Boolean> val = Boolean::New(isolate, false);
            args.GetReturnValue().Set(val);
            return;
        }
        generateKey(keyChr, sizeFile);
        bool result = VernamCipher(inChr, outChr, keyChr);
        if (result)
        {
            Local<Boolean> val = Boolean::New(isolate, true);
            args.GetReturnValue().Set(val);
        }
        else
        {
            Local<Boolean> val = Boolean::New(isolate, false);
            args.GetReturnValue().Set(val);
        }
    }

    void Initialize(Local<Object> exports)
    {
        NODE_SET_METHOD(exports, "cipher", Method);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
}