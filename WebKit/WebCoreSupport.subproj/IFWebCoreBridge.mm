/*	
    IFWebCoreBridge.mm
	Copyright (c) 2002, Apple, Inc. All rights reserved.
*/

#import <WebKit/IFWebCoreBridge.h>

#import <WebKit/IFHTMLRepresentationPrivate.h>
#import <WebKit/IFResourceURLHandleClient.h>
#import <WebKit/IFWebControllerPrivate.h>
#import <WebKit/IFWebCoreFrame.h>
#import <WebKit/IFWebDataSourcePrivate.h>
#import <WebKit/IFWebFramePrivate.h>
#import <WebKit/IFWebViewPrivate.h>
#import <WebKit/IFLoadProgress.h>

#import <WebKit/WebKitDebug.h>

#import <WebFoundation/IFURLHandle.h>

@implementation IFWebDataSource (IFWebCoreBridge)

- (IFWebCoreBridge *)_bridge
{
    id representation = [self representation];
    return [representation respondsToSelector:@selector(_bridge)] ? [representation _bridge] : nil;
}

@end

@interface NSApplication (DeclarationStolenFromAppKit)
- (void)_cycleWindowsReversed:(BOOL)reversed;
@end

@implementation IFWebCoreBridge

- (id <WebCoreFrame>)frame
{
    return [[dataSource webFrame] _bridgeFrame];
}

- (WebCoreBridge *)parent
{
    WEBKIT_ASSERT(dataSource);
    return [[dataSource parent] _bridge];
}

- (NSArray *)childFrames
{
    WEBKIT_ASSERT(dataSource);
    NSArray *frames = [dataSource children];
    NSEnumerator *e = [frames objectEnumerator];
    NSMutableArray *bridgeFrames = [NSMutableArray arrayWithCapacity:[frames count]];
    IFWebFrame *frame;
    while ((frame = [e nextObject])) {
        id bridgeFrame = [frame _bridgeFrame];
        if (bridgeFrame)
            [bridgeFrames addObject:bridgeFrame];
    }
    return bridgeFrames;
}

- (id <WebCoreFrame>)childFrameNamed:(NSString *)name
{
    return [[dataSource frameNamed:name] _bridgeFrame];
}

- (id <WebCoreFrame>)descendantFrameNamed:(NSString *)name
{
    return [[[dataSource webFrame] frameNamed:name] _bridgeFrame];
}

- (BOOL)createChildFrameNamed:(NSString *)frameName
    withURL:(NSURL *)URL renderPart:(khtml::RenderPart *)renderPart
    allowsScrolling:(BOOL)allowsScrolling marginWidth:(int)width marginHeight:(int)height
{
    WEBKIT_ASSERT(dataSource);

    IFWebFrame *frame = [[dataSource controller] createFrameNamed:frameName for:nil inParent:dataSource allowsScrolling:allowsScrolling];
    if (frame == nil) {
        return NO;
    }
    
    [frame _setRenderFramePart:renderPart];
    
    [[frame webView] _setMarginWidth:width];
    [[frame webView] _setMarginHeight:height];

    [[frame _bridgeFrame] loadURL:URL attributes:nil flags:0 withParent:dataSource];
    
    return YES;
}

- (WebCoreBridge *)openNewWindowWithURL:(NSURL *)url
{
    IFWebController *newController = [[[dataSource controller] windowContext] openNewWindowWithURL:url];
    IFWebDataSource *newDataSource;
    
    newDataSource = [[newController mainFrame] provisionalDataSource];
    if ([newDataSource isDocumentHTML])
        return [(IFHTMLRepresentation *)[newDataSource representation] _bridge];
        
    return nil;
}

- (BOOL)areToolbarsVisisble
{
    return [[[dataSource controller] windowContext] areToolbarsVisible];
}

- (void)setToolbarsVisible:(BOOL)visible
{
    [[[dataSource controller] windowContext] setToolbarsVisible:visible];
}

- (BOOL)areScrollbarsVisible
{
    return [[[dataSource webFrame] webView] allowsScrolling];
}

- (void)setScrollbarsVisible:(BOOL)visible
{
    return [[[dataSource webFrame] webView] setAllowsScrolling:visible];
}

- (BOOL)isStatusBarVisisble
{
    return [[[dataSource controller] windowContext] isStatusBarVisible];
}

- (void)setStatusBarVisible:(BOOL)visible
{
    [[[dataSource controller] windowContext] setStatusBarVisible:visible];
}

- (void)setWindowFrame:(NSRect)frame
{
    [[[dataSource controller] windowContext] setFrame:frame];
}


- (NSWindow *)window
{
    return [[[dataSource controller] windowContext] window];
}

- (void)setTitle:(NSString *)title
{
    WEBKIT_ASSERT(dataSource);
    [dataSource _setTitle:title];
}

- (void)setStatusText:(NSString *)status
{
    WEBKIT_ASSERT(dataSource);
    [[[dataSource controller] windowContext] setStatusText:status];
}

- (id <WebCoreFrame>)mainFrame
{
    return [[[dataSource controller] mainFrame] _bridgeFrame];
}

- (id <WebCoreFrame>)frameNamed:(NSString *)name
{
    return [[[dataSource controller] frameNamed:name] _bridgeFrame];
}

- (void)receivedData:(NSData *)data withDataSource:(IFWebDataSource *)withDataSource
{
    if (dataSource == nil) {
        [self setDataSource: withDataSource];
        [self openURL:[dataSource inputURL]];
        if ([dataSource redirectedURL]) {
            [self setURL:[dataSource redirectedURL]];
        }
    } else {
        WEBKIT_ASSERT(dataSource == withDataSource);
    }
    
    [self addData:data withEncoding:[dataSource encoding]];
}

- (IFURLHandle *)startLoadingResource:(id <WebCoreResourceLoader>)resourceLoader withURL:(NSURL *)URL
{
    return [IFResourceURLHandleClient startLoadingResource:resourceLoader withURL:URL dataSource:dataSource];
}

- (void)objectLoadedFromCache:(NSURL *)URL size:(unsigned)bytes
{
    IFURLHandle *handle;
    IFLoadProgress *loadProgress;
    
    handle = [[IFURLHandle alloc] initWithURL:URL];
    loadProgress = [[IFLoadProgress alloc] initWithBytesSoFar:bytes totalToLoad:bytes];
    [[dataSource controller] _receivedProgress:loadProgress forResourceHandle:handle fromDataSource: dataSource complete:YES];
    [loadProgress release];
    [handle release];
}

- (void)setDataSource: (IFWebDataSource *)ds
{
    // FIXME: non-retained because data source owns representation owns bridge
    dataSource = ds;
}

- (BOOL)openedByScript
{
    return [[dataSource controller] _openedByScript];
}

- (void)setOpenedByScript:(BOOL)openedByScript
{
    [[dataSource controller] _setOpenedByScript:openedByScript];
}

- (void)unfocusWindow
{
    if ([[self window] isKeyWindow] || [[[self window] attachedSheet] isKeyWindow]) {
	[NSApp _cycleWindowsReversed:FALSE];
    }
}

@end
