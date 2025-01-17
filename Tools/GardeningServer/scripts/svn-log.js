/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

var trac = trac || {};

(function() {

function findUsingRegExp(string, regexp)
{
    var match = regexp.exec(string);
    if (match)
        return match[1];
    return null;
}

function findReviewer(message)
{
    var regexp = /R=([^.]+)/;
    return findUsingRegExp(message, regexp);
}

function findBugID(message)
{
    var regexp = /BUG=(\d+)/;
    return parseInt(findUsingRegExp(message, regexp), 10);
}

function parseCommitMessage(message) {
    var lines = message.split('\n');
    var title = lines.shift();
    var summary = lines.join('\n').trim();
    return {
        title: title,
        summary: summary,
        bugID: findBugID(summary),
        reviewer: findReviewer(summary),
    }
}

// FIXME: Consider exposing this method for unit testing.
function parseCommitData(responseXML)
{
    var commits = Array.prototype.map.call(responseXML.getElementsByTagName('logentry'), function(logentry) {
        var author = logentry.getElementsByTagName('author')[0].textContent;
        var time = logentry.getElementsByTagName('date')[0].textContent;

        // FIXME: This isn't a very high-fidelity reproduction of the commit message,
        // but it's good enough for our purposes.
        var message = parseCommitMessage(logentry.getElementsByTagName('msg')[0].textContent);

        return {
            'revision': logentry.getAttribute('revision'),
            'title': message.title,
            'time': time,
            'summary': message.title,
            'author': author,
            'reviewer': message.reviewer,
            'bugID': message.bugID,
            'message': message.summary,
            'revertedRevision': undefined,
        };
    });
    return commits;
}

trac.changesetURL = function(revision)
{
    var queryParameters = {
        view: 'rev',
        revision: revision,
    };
    return config.kBlinkRevisionURL + '?' + $.param(queryParameters);
};

trac.recentCommitData = function(path, limit, callback)
{
    net.get('/svnlog', function(commitData) {
        callback(parseCommitData(commitData));
    });
};

trac.commitDataForRevisionRange = function(path, startRevision, endRevision, callback)
{
    var key = [path, startRevision, endRevision].join('\n');
    g_cache.get(key, callback);
};

})();
