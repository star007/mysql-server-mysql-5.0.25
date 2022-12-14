/* server.cpp */


#include "../../testsuite/test.hpp"


THREAD_RETURN YASSL_API server_test(void* args)
{
#ifdef _WIN32
    WSADATA wsd;
    WSAStartup(0x0002, &wsd);
#endif

    SOCKET_T sockfd   = 0;
    int      clientfd = 0;
    int      argc     = 0;
    char**   argv     = 0;

    set_args(argc, argv, *static_cast<func_args*>(args));
    tcp_accept(sockfd, clientfd, *static_cast<func_args*>(args));

#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif

    SSL_METHOD* method = TLSv1_server_method();
    SSL_CTX*    ctx = SSL_CTX_new(method);

    //SSL_CTX_set_cipher_list(ctx, "RC4-SHA");
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);
    set_serverCerts(ctx);
    DH* dh = set_tmpDH(ctx);

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, clientfd);
   
    if (SSL_accept(ssl) != SSL_SUCCESS) err_sys("SSL_accept failed");
    showPeer(ssl);
    printf("Using Cipher Suite %s\n", SSL_get_cipher(ssl));

    char command[1024];
    command[SSL_read(ssl, command, sizeof(command))] = 0;
    printf("First client command: %s\n", command);

    char msg[] = "I hear you, fa shizzle!";
    if (SSL_write(ssl, msg, sizeof(msg)) != sizeof(msg))
        err_sys("SSL_write failed"); 

    DH_free(dh);
    SSL_CTX_free(ctx);
    SSL_free(ssl);

    ((func_args*)args)->return_code = 0;
    return 0;
}


#ifndef NO_MAIN_DRIVER

    int main(int argc, char** argv)
    {
        func_args args;

        args.argc = argc;
        args.argv = argv;

        server_test(&args);
        yaSSL_CleanUp();

        return args.return_code;
    }

#endif // NO_MAIN_DRIVER
