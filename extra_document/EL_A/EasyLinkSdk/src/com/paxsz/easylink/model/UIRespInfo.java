package com.paxsz.easylink.model;

/**
 * Created by wangcy on 2017/7/7.
 */

public class UIRespInfo {

    private byte[] actionData;
    private EUIActionType actionType;

    public byte[] getActionData() {
        return actionData;
    }
    public void setActionData(byte[] actionData) {
        this.actionData = actionData;
    }
    public enum EUIActionType{
        UI_ACTION_SIGNATURE(4),
        UI_ACTION_COMMENT_STAR(5);

        private final int action;

        EUIActionType(int action) {
            this.action = action;
        }
    }
    public EUIActionType getType() {
        return actionType;
    }
    public void setType(EUIActionType actionType) {
        this.actionType = actionType;
    }
}